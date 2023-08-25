//===---------------------------------------------------------===
//
// This file contains core types for compatability accross
// the ssc compiler.
//
//===---------------------------------------------------------===
#ifndef SSC_CORE_TYPES_H
#define SSC_CORE_TYPES_H

#include <cstdint>
#include <cstddef> // size_t

using ulen = size_t;

// for working with bits of specific sizes.
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

#endif

