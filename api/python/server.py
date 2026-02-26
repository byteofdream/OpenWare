import json
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import parse_qs, urlparse

from c_api_bridge import c_api_sum


class ApiHandler(BaseHTTPRequestHandler):
    def _send_json(self, payload: dict, status: int = 200) -> None:
        body = json.dumps(payload).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self) -> None:
        parsed = urlparse(self.path)
        query = parse_qs(parsed.query)

        if parsed.path == "/health":
            self._send_json({"status": "ok"})
            return

        if parsed.path == "/version":
            self._send_json({"version": "0.1.0"})
            return

        if parsed.path == "/sum":
            try:
                a = int(query.get("a", ["0"])[0])
                b = int(query.get("b", ["0"])[0])
                result = c_api_sum(a, b)
                self._send_json({"a": a, "b": b, "result": result})
            except (ValueError, FileNotFoundError) as exc:
                self._send_json({"error": str(exc)}, status=400)
            return

        self._send_json({"error": "not found"}, status=404)


def run(host: str = "127.0.0.1", port: int = 8080) -> None:
    httpd = HTTPServer((host, port), ApiHandler)
    print(f"OpenWare API server running at http://{host}:{port}")
    httpd.serve_forever()


if __name__ == "__main__":
    run()

