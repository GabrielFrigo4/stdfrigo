#ifndef STDHASH_H
#define STDHASH_H

#include <stdfrigo_defs.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===============================================================
 * INTEGER HASH MIXERS
 * =============================================================== */

uint32_t hash32_int(uint32_t num);
uint64_t hash64_int(uint64_t num);

/* ===============================================================
 * MEMORY HASH (WyHash Variant)
 * =============================================================== */

uint64_t hash64_mem(const void *mem, size_t size);
uint32_t hash32_mem(const void *mem, size_t size);

/* ===============================================================
 * AUXILIAR: COMBINE HASH
 * =============================================================== */

uint32_t hash32_combine(uint32_t seed, uint32_t next_hash);
uint64_t hash64_combine(uint64_t seed, uint64_t next_hash);

/* ===============================================================
 * HARDWARE HASHING (X86-64 ONLY)
 * =============================================================== */

#if defined(__x86_64__) || defined(_M_X64)
bool hash32_hw(const void *mem, size_t size, uint32_t *out);
bool hash64_hw(const void *mem, size_t size, uint64_t *out);
#endif

#ifdef __cplusplus
}
#endif

/* ===============================================================
 * API GENÉRICA (POLIMORFISMO)
 * =============================================================== */

/* ===============================================================
 * IMPLEMENTAÇÃO C++ (Function Overloading)
 * =============================================================== */

#ifdef __cplusplus
static inline uint32_t hash_int(uint32_t num) {
    return hash32_int(num);
}
static inline uint64_t hash_int(uint64_t num) {
    return hash64_int(num);
}

template <typename T> static inline auto hash_int(T num) {
    if constexpr (sizeof(T) <= 4) {
        return hash32_int(static_cast<uint32_t>(num));
    } else {
        return hash64_int(static_cast<uint64_t>(num));
    }
}

static inline void hash_mem(const void *mem, size_t size, uint32_t *out) {
    *out = hash32_mem(mem, size);
}
static inline void hash_mem(const void *mem, size_t size, uint64_t *out) {
    *out = hash64_mem(mem, size);
}

static inline uint32_t hash_combine(uint32_t seed, uint32_t next_hash) {
    return hash32_combine(seed, next_hash);
}
static inline uint64_t hash_combine(uint64_t seed, uint64_t next_hash) {
    return hash64_combine(seed, next_hash);
}

#if defined(__x86_64__) || defined(_M_X64)
static inline bool hash_hw(const void *mem, size_t size, uint32_t *out) {
    return hash32_hw(mem, size, out);
}
static inline bool hash_hw(const void *mem, size_t size, uint64_t *out) {
    return hash64_hw(mem, size, out);
}
#endif
#endif

/* ===============================================================
 * IMPLEMENTAÇÃO C11 (_Generic Macro)
 * =============================================================== */

// clang-format off
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define hash_int(num) _Generic((num), \
    uint64_t: hash64_int,             \
    int64_t:  hash64_int,             \
    default:  _Generic((num),         \
        uint32_t: hash32_int,         \
        int32_t:  hash32_int,         \
        default:  sizeof(num) > 4 ? hash64_int((uint64_t)(num)) : hash32_int((uint32_t)(num)) \
    )                                 \
)(num)

#define hash_mem(mem, size, out) _Generic((out),        \
    uint32_t *: (void)(*(out) = hash32_mem(mem, size)), \
    uint64_t *: (void)(*(out) = hash64_mem(mem, size))  \
)

#define hash_combine(seed, next_hash) _Generic((seed), \
    uint32_t: hash32_combine,                          \
    uint64_t: hash64_combine                           \
)(seed, next_hash)

#if defined(__x86_64__) || defined(_M_X64)
#define hash_hw(mem, size, out) _Generic((out),  \
    uint32_t *: hash32_hw,                       \
    uint64_t *: hash64_hw                        \
)(mem, size, out)
#endif
#endif
// clang-format on

#endif
