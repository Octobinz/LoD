#pragma once

#include <stdint.h>
#include <cstddef>

#ifdef _WINDLL
#ifdef __cplusplus
extern "C" {
#endif
#endif

#include <pd_api.h>

#ifdef _WINDLL
#ifdef __cplusplus
}
#endif
#endif

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef size_t   usize;

//#define FORCEINLINE __forceinline
#ifdef _MSC_VER
#include <stdlib.h>
#define bswap_16(x) _byteswap_ushort(x)
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)

#define FORCEINLINE __forceinline
#else

#define bswap_16(x) __builtin_bswap16(x)
#define bswap_32(x) __builtin_bswap32(x)
#define bswap_64(x) __builtin_bswap64(x)
#define FORCEINLINE __attribute__((inline))

#endif