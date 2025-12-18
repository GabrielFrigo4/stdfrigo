#ifndef STDHASH_H
#define STDHASH_H

#ifndef STDFRIGO_LIB
#define STDFRIGO_LIB
#ifdef _MSC_VER
#pragma comment(lib, "libstdfrigo.a")
#endif
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===============================================================
 * INTEGER HASH MIXERS
 * =============================================================== */

uint32_t hash32_int(uint32_t k);
uint64_t hash64_int(uint64_t k);

/* ===============================================================
 * FAST HASH (WyHash Variant)
 * =============================================================== */

uint64_t hash64_fast(const void *buff, size_t len);
uint32_t hash32_fast(const void *buff, size_t len);

/* ===============================================================
 * AUXILIAR: COMBINE HASH
 * =============================================================== */

uint32_t hash32_combine(uint32_t seed, uint32_t next_hash);
uint64_t hash64_combine(uint64_t seed, uint64_t next_hash);

/* ===============================================================
 * HARDWARE HASHING (X86-64 ONLY)
 * =============================================================== */

#if defined(__x86_64__) || defined(_M_X64)
bool hash32_hw(const void *buff, size_t len, uint32_t *out);
bool hash64_hw(const void *buff, size_t len, uint64_t *out);
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
static inline uint32_t hash_int(uint32_t k) {
    return hash32_int(k);
}
static inline uint64_t hash_int(uint64_t k) {
    return hash64_int(k);
}

template <typename T> static inline auto hash_int(T k) {
    if constexpr (sizeof(T) <= 4) {
        return hash32_int(static_cast<uint32_t>(k));
    } else {
        return hash64_int(static_cast<uint64_t>(k));
    }
}

static inline void hash_fast(const void *b, size_t l, uint32_t *out) {
    *out = hash32_fast(b, l);
}
static inline void hash_fast(const void *b, size_t l, uint64_t *out) {
    *out = hash64_fast(b, l);
}

static inline uint32_t hash_combine(uint32_t s, uint32_t v) {
    return hash32_combine(s, v);
}
static inline uint64_t hash_combine(uint64_t s, uint64_t v) {
    return hash64_combine(s, v);
}

#if defined(__x86_64__) || defined(_M_X64)
static inline bool hash_hw(const void *b, size_t l, uint32_t *out) {
    return hash32_hw(b, l, out);
}
static inline bool hash_hw(const void *b, size_t l, uint64_t *out) {
    return hash64_hw(b, l, out);
}
#endif
#endif

/* ===============================================================
 * IMPLEMENTAÇÃO C11 (_Generic Macro)
 * =============================================================== */

// clang-format off
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define hash_int(k) _Generic((k), \
    uint64_t: hash64_int,         \
    int64_t:  hash64_int,         \
    default:  _Generic((k),       \
        uint32_t: hash32_int,     \
        int32_t:  hash32_int,     \
        default:  sizeof(k) > 4 ? hash64_int((uint64_t)(k)) : hash32_int((uint32_t)(k)) \
    )                             \
)(k)

#define hash_fast(buff, len, out) _Generic((out),                    \
    uint32_t *: (void)(*(uint32_t *)(out) = hash32_fast(buff, len)), \
    uint64_t *: (void)(*(uint64_t *)(out) = hash64_fast(buff, len))  \
)

#define hash_combine(s, v) _Generic((s), \
    uint32_t: hash32_combine,            \
    uint64_t: hash64_combine             \
)(s, v)

#if defined(__x86_64__) || defined(_M_X64)
#define hash_hw(buff, len, out) _Generic((out), \
    uint32_t *: hash32_hw,                      \
    uint64_t *: hash64_hw                       \
)(buff, len, out)
#endif
#endif
// clang-format on

#endif
