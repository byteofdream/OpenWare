#ifndef OPENWARE_API_H
#define OPENWARE_API_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) && defined(OW_API_EXPORTS)
#define OW_API __declspec(dllexport)
#elif defined(_WIN32)
#define OW_API __declspec(dllimport)
#else
#define OW_API
#endif

OW_API const char* ow_api_version(void);
OW_API int ow_api_sum(int a, int b);
OW_API int ow_api_is_ready(void);

#ifdef __cplusplus
}
#endif

#endif

