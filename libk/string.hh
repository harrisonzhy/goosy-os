#pragma once
#include <stddef.h>

auto strlen(const char* str) -> size_t {
    size_t len = 0;
    while(str[len]) {
        ++len;
    }
    return len;
}