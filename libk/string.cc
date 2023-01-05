#include "string.hh"

auto memcmp(const void* s1, const void* s2, size_t n) -> int {

}

auto memcpy(void* __restrict dst, const void* __restrict src, size_t n) -> void* {

}

auto memmove(void *dst, const void *src, size_t len) -> void* {

}

auto memset(void *b, int c, size_t len) -> void* {

}

auto strlen(const char* str) -> size_t {
    size_t len = 0;
    while (str[len]) {
        ++len;
    }
    return len;
}