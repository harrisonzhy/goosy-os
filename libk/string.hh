#pragma once
#include <sys/cdefs.h>
#include <stddef.h>
#include <stdint.h>

auto memcmp(const void* s1, const void* s2, size_t n) -> int {
    // assumes both strings are size `n`
    auto f = (uint8_t*)s1;
    auto s = (uint8_t*)s2;
    for (auto i = 0; i != n; ++i) {
        if (f[i] < s[i]) {
            return -1;
        }
        else if (f[i] > s[i]) {
            return 1;
        }
    }
    return 0;
}

auto memcpy(void* __restrict dst, const void* __restrict src, size_t n) -> void* {
    auto s = (const uint8_t*)src;
    auto d = (uint8_t*)dst;
    for (auto i = 0; i != n; ++i) {
        d[i] = s[i];
    }
    return dst;
}

auto memmove(void *dst, const void *src, size_t len) -> void* {
    if (dst == src || len == 0) {
        return dst;
    }
    else if (dst < src) {
        auto s = (const uint8_t*)src;
        auto d = (uint8_t*)dst;
        for (auto i = 0; i != len; ++i) {
            d[i] = s[i];
        }
    }
    else {
        auto ls = (const uint8_t*)src+(len-1);
        auto ld = (uint8_t*)dst+(len-1);
        for (auto j = len; j != 0; --j) {
            ld[j-1] = ls[j-1];
        }
    }
    return dst;
}

auto memset(void *b, int c, size_t len) -> void* {
    auto dst = (uint8_t*)b;
    for (auto i = 0; i != len; ++i) {
        dst[i] = c;
    }
    return b;
}

auto strlen(const int8_t* str) -> size_t {
    size_t len = 0;
    while (str[len]) {
        ++len;
    }
    return len;
}