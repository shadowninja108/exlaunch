/**
 * @file switch/types.h
 * @brief Various system types.
 * @copyright libnx Authors
 */
#pragma once

#include "../../types.h"

#include <stddef.h>
#include <stdalign.h>

#include "result.h"

/// The maximum value of a u64.
#define U64_MAX	UINT64_MAX

#ifndef SSIZE_MAX
#ifdef SIZE_MAX
#define SSIZE_MAX ((SIZE_MAX) >> 1)
#endif
#endif

typedef u32 Handle;                 ///< Kernel object handle.
typedef u32 Result;                 ///< Function error code result type.
typedef void (*ThreadFunc)(void *); ///< Thread entrypoint function.
typedef void (*VoidFn)(void);       ///< Function without arguments nor return value.

/// Creates a bitmask from a bit number.
#ifndef BIT
#define BIT(n) (1U<<(n))
#endif

/// Packs a struct so that it won't include padding bytes.
#ifndef PACKED
#define PACKED     __attribute__((packed))
#endif

/// Marks a function as not returning, for the purposes of compiler optimization.
#ifndef NORETURN
#define NORETURN   __attribute__((noreturn))
#endif

/// Performs a dummy operation on the specified argument in order to silence compiler warnings about unused arguments.
#ifndef IGNORE_ARG
#define IGNORE_ARG(x) (void)(x)
#endif

/// Flags a function as deprecated.
#ifndef DEPRECATED
#ifndef LIBNX_NO_DEPRECATION
#define DEPRECATED __attribute__ ((deprecated))
#else
#define DEPRECATED
#endif
#endif

/// Flags a function as (always) inline.
#define NX_INLINE __attribute__((always_inline)) static inline

/// Flags a function as constexpr in C++14 and above; or as (always) inline otherwise.
#if __cplusplus >= 201402L
#define NX_CONSTEXPR NX_INLINE constexpr
#else
#define NX_CONSTEXPR NX_INLINE
#endif

/// Invalid handle.
#define INVALID_HANDLE ((Handle) 0)
