#pragma once
#include <stddef.h>
#include <stdint.h>

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef uintptr_t uptr;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef size_t  usize;
typedef int32_t isize;

constexpr u8    operator "" _u8    (unsigned long long int i) { return u8(i);    }
constexpr u16   operator "" _u16   (unsigned long long int i) { return u16(i);   }
constexpr u32   operator "" _u32   (unsigned long long int i) { return u32(i);   }
constexpr u64   operator "" _u64   (unsigned long long int i) { return u64(i);   }
constexpr uptr  operator "" _uptr  (unsigned long long int i) { return uptr(i); }

constexpr i8    operator "" _i8    (unsigned long long int i) { return i8(i);    }
constexpr i16   operator "" _i16   (unsigned long long int i) { return i16(i);   }
constexpr i32   operator "" _i32   (unsigned long long int i) { return i32(i);   }
constexpr i64   operator "" _i64   (unsigned long long int i) { return i64(i);   }

constexpr usize operator "" _usize (unsigned long long int i) { return usize(i); }
constexpr isize operator "" _isize (unsigned long long int i) { return isize(i); }