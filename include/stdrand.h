#ifndef STDRAND_H
#define STDRAND_H

#include <stdfrigo_defs.h>

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===============================================================
 * ESTRUTURAS DE ESTADO (PRNGs)
 * =============================================================== */

typedef struct rand32 {
    uint32_t s[4];
} rand32_t;

typedef struct rand64 {
    uint64_t s[4];
} rand64_t;

typedef struct rand_float {
    uint32_t s[4];
} rand_float_t;

typedef struct rand_double {
    uint64_t s[2];
} rand_double_t;

/* ===============================================================
 * PROTÓTIPOS DE INICIALIZAÇÃO E GERAÇÃO
 * =============================================================== */

rand32_t rand32_init(uint64_t seed);
uint32_t rand32_next(rand32_t *rng);

rand64_t rand64_init(uint64_t seed);
uint64_t rand64_next(rand64_t *rng);

rand_float_t rand_float_init(uint64_t seed);
float rand_float_next(rand_float_t *rng);

rand_double_t rand_double_init(uint64_t seed);
double rand_double_next(rand_double_t *rng);

/* ===============================================================
 * PROTÓTIPOS DE UTILIDADE
 * =============================================================== */

void rand32_seed(rand32_t *rng, uint64_t seed);
void rand64_seed(rand64_t *rng, uint64_t seed);
void rand_float_seed(rand_float_t *rng, uint64_t seed);
void rand_double_seed(rand_double_t *rng, uint64_t seed);

void rand32_jump(rand32_t *rng);
void rand64_jump(rand64_t *rng);
void rand_float_jump(rand_float_t *rng);
void rand_double_jump(rand_double_t *rng);

uint32_t rand32_bound(rand32_t *rng, uint32_t limit);
uint64_t rand64_bound(rand64_t *rng, uint64_t limit);
float rand_float_bound(rand_float_t *rng, float limit);
double rand_double_bound(rand_double_t *rng, double limit);

uint32_t rand32_range(rand32_t *rng, uint32_t min, uint32_t max);
uint64_t rand64_range(rand64_t *rng, uint64_t min, uint64_t max);
float rand_float_range(rand_float_t *rng, float min, float max);
double rand_double_range(rand_double_t *rng, double min, double max);

/* ===============================================================
 * HARDWARE RANDOM (x86-64 ONLY)
 * =============================================================== */

#if defined(__x86_64__) || defined(_M_X64)
bool rand32_hw_fast(uint32_t *out);
bool rand64_hw_fast(uint64_t *out);
bool rand32_hw_entropy(uint32_t *out);
bool rand64_hw_entropy(uint64_t *out);
uint32_t rand32_hw_seed(void);
uint64_t rand64_hw_seed(void);
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
static inline uint32_t rand_next(rand32_t *rng) {
    return rand32_next(rng);
}
static inline uint64_t rand_next(rand64_t *rng) {
    return rand64_next(rng);
}
static inline uint32_t rand_next(rand_float_t *rng) {
    return rand_float_next(rng);
}
static inline uint64_t rand_next(rand_double_t *rng) {
    return rand_double_next(rng);
}

static inline void rand_init(uint64_t seed, rand32_t *out) {
    *out = rand32_init(seed);
}
static inline void rand_init(uint64_t seed, rand64_t *out) {
    *out = rand64_init(seed);
}
static inline void rand_init(uint64_t seed, rand_float_t *out) {
    *out = rand_float_init(seed);
}
static inline void rand_init(uint64_t seed, rand_double_t *out) {
    *out = rand_double_init(seed);
}

static inline void rand_seed(rand32_t *rng, uint64_t seed) {
    rand32_seed(rng, seed);
}
static inline void rand_seed(rand64_t *rng, uint64_t seed) {
    rand64_seed(rng, seed);
}
static inline void rand_seed(rand_float_t *rng, uint64_t seed) {
    rand_float_seed(rng, seed);
}
static inline void rand_seed(rand_double_t *rng, uint64_t seed) {
    rand_double_seed(rng, seed);
}

static inline void rand_jump(rand32_t *rng) {
    rand32_jump(rng);
}
static inline void rand_jump(rand64_t *rng) {
    rand64_jump(rng);
}
static inline void rand_jump(rand_float_t *rng) {
    rand_float_jump(rng);
}
static inline void rand_jump(rand_double_t *rng) {
    rand_double_jump(rng);
}

static inline uint32_t rand_bound(rand32_t *rng, uint32_t limit) {
    return rand32_bound(rng, limit);
}
static inline uint64_t rand_bound(rand64_t *rng, uint64_t limit) {
    return rand64_bound(rng, limit);
}
static inline float rand_bound(rand_float_t *rng, float limit) {
    return rand_float_bound(rng, limit);
}
static inline double rand_bound(rand_double_t *rng, double limit) {
    return rand_double_bound(rng, limit);
}

static inline uint32_t rand_range(rand32_t *rng, uint32_t min, uint32_t max) {
    return rand32_range(rng, min, max);
}
static inline uint64_t rand_range(rand64_t *rng, uint64_t min, uint64_t max) {
    return rand64_range(rng, min, max);
}
static inline float rand_range(rand_float_t *rng, float min, float max) {
    return rand_float_range(rng, min, max);
}
static inline double rand_range(rand_double_t *rng, double min, double max) {
    return rand_double_range(rng, min, max);
}

#if defined(__x86_64__) || defined(_M_X64)
static inline uint32_t rand_hw_fast(uint32_t *out) {
    return rand32_hw_fast(out);
}
static inline uint64_t rand_hw_fast(uint64_t *out) {
    return rand64_hw_fast(out);
}
static inline uint32_t rand_hw_entropy(uint32_t *out) {
    return rand32_hw_entropy(out);
}
static inline uint64_t rand_hw_entropy(uint64_t *out) {
    return rand64_hw_entropy(out);
}
static inline void rand_hw_seed(uint32_t *out) {
    *out = rand32_hw_seed();
}
static inline void rand_hw_seed(uint64_t *out) {
    *out = rand64_hw_seed();
}
#endif
#endif

/* ===============================================================
 * IMPLEMENTAÇÃO C11 (_Generic Macro)
 * =============================================================== */

// clang-format off
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define rand_next(rng) _Generic((rng), \
    rand32_t *:      rand32_next,      \
    rand64_t *:      rand64_next,      \
    rand_float_t *:  rand_float_next,  \
    rand_double_t *: rand_double_next  \
)(rng)

#define rand_init(seed, out) _Generic((out), \
    rand32_t *:      (void)(*out = rand32_init(seed)),      \
    rand64_t *:      (void)(*out = rand64_init(seed)),      \
    rand_float_t *:  (void)(*out = rand_float_init(seed)),  \
    rand_double_t *: (void)(*out = rand_double_init(seed))  \
)

#define rand_seed(rng, seed) _Generic((rng), \
    rand32_t *:      rand32_seed,            \
    rand64_t *:      rand64_seed,            \
    rand_float_t *:  rand_float_seed,        \
    rand_double_t *: rand_double_seed        \
)(rng, seed)

#define rand_jump(rng) _Generic((rng), \
    rand32_t *:      rand32_jump,      \
    rand64_t *:      rand64_jump,      \
    rand_float_t *:  rand_float_jump,  \
    rand_double_t *: rand_double_jump  \
)(rng)

#define rand_bound(rng, limit) _Generic((rng), \
    rand32_t *:      rand32_bound,             \
    rand64_t *:      rand64_bound,             \
    rand_float_t *:  rand_float_bound,         \
    rand_double_t *: rand_double_bound         \
)(rng, limit)

#define rand_range(rng, min, max) _Generic((rng), \
    rand32_t *:      rand32_range,                \
    rand64_t *:      rand64_range,                \
    rand_float_t *:  rand_float_range,            \
    rand_double_t *: rand_double_range            \
)(rng, min, max)

#if defined(__x86_64__) || defined(_M_X64)
#define rand_hw_fast(out) _Generic((out), \
    uint32_t *: rand32_hw_fast,           \
    uint64_t *: rand64_hw_fast            \
)(out)

#define rand_hw_entropy(out) _Generic((out), \
    uint32_t *: rand32_hw_entropy,           \
    uint64_t *: rand64_hw_entropy            \
)(out)

#define rand_hw_seed(out) _Generic((out),        \
    uint32_t *: (void)(*out = rand32_hw_seed()), \
    uint64_t *: (void)(*out = rand64_hw_seed())  \
)
#endif
#endif
// clang-format on

#endif
