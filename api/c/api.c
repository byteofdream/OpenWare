#include "api.h"

const char* ow_api_version(void) {
    return "0.1.0";
}

int ow_api_sum(int a, int b) {
    return a + b;
}

int ow_api_is_ready(void) {
    return 1;
}

