#pragma once
#include "int.hh"

// auto memcmp(const void* s1, const void* s2, usize n) -> int {
//     // assumes both strings are size `n`
//     auto f = (u8*)s1;
//     auto s = (u8*)s2;
//     for (auto i = 0; i != n; ++i) {
//         if (f[i] < s[i]) {
//             return -1;
//         }
//         else if (f[i] > s[i]) {
//             return 1;
//         }
//     }
//     return 0;
// }

// auto memcpy(void* __restrict dst, const void* __restrict src, usize n) -> void* {
//     auto s = (const u8*)src;
//     auto d = (u8*)dst;
//     for (auto i = 0; i != n; ++i) {
//         d[i] = s[i];
//     }
//     return dst;
// }

// auto memmove(void *dst, const void *src, usize len) -> void* {
//     if (dst == src || len == 0) {
//         return dst;
//     }
//     else if (dst < src) {
//         auto s = (const u8*)src;
//         auto d = (u8*)dst;
//         for (auto i = 0; i != len; ++i) {
//             d[i] = s[i];
//         }
//     }
//     else {
//         auto ls = (const u8*)src+(len-1);
//         auto ld = (u8*)dst+(len-1);
//         for (auto j = len; j != 0; --j) {
//             ld[j-1] = ls[j-1];
//         }
//     }
//     return dst;
// }

// auto memset(void *b, int c, usize len) -> void* {
//     auto dst = (u8*)b;
//     for (auto i = 0; i != len; ++i) {
//         dst[i] = c;
//     }
//     return b;
// }

auto strlen(const char* str) -> usize {
    usize len = 0;
    while (str[len]) {
        ++len;
    }
    return len;
}
