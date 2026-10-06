#ifndef ZN_TYPES_H
#define ZN_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef ptrdiff_t isz;
typedef size_t usz;

// U8Vec { data: *u8, len: u32, cap: u32 }
typedef struct U8Vec {
  u8* data;
  u32 len;
  u32 cap;
} U8Vec;

// U32Vec { data: *u32, len: u32, cap: u32 }
typedef struct U32Vec {
  u32* data;
  u32 len;
  u32 cap;
} U32Vec;

// ZNative Defaults
enum : u32 {
  ZND_TOKBUF_CAPACITY = 128,
  ZND_NLBUF_CAPACITY = 64,
};

// ZNative Status Code
typedef enum ZNStatus : u32 {
  ZNS_OK = 0,
  ZNS_FILE_OPEN_ERR,
  ZNS_FILE_INVALID_ERR,
  ZNS_NULL_READ_ERR,
  ZNS_ALLOC_ERR,
  ZNS_LEX_ERR,
} zns;

#endif