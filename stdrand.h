#ifndef STDRAND_H
#define STDRAND_H

#include <stdint.h>

/* ===============================================================
 MACROS DE CONVERSÃO PARA PONTO FLUTUANTE
 Converte inteiros aleatórios para o intervalo [0, 1).

 A lógica consiste em manter apenas os bits da mantissa (53 ou 24)
 e multiplicar pelo inverso da potência de 2 correspondente.
 =============================================================== */

#define TO_DBL_01(u) \
    (((u) >> 11) * 0x1.0p-53)

#define TO_FLT_01(u) \
    (((u) >> 8) * 0x1.0p-24f)

/* ===============================================================
 A LGORITMOS AUXILIARES (SPLITMIX)
 Usados para inicializar os estados de forma segura.
 =============================================================== */

static inline uint64_t splitmix64_next(uint64_t *state)
{
    uint64_t z = (*state += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

/* ===============================================================
 X ORSHIFT 32 BITS*
 Periodo: 2^32 - 1
 =============================================================== */

typedef struct rand32
{
    uint32_t state;
} rand32_t;

static inline void rand32_seed(rand32_t *rand32, uint64_t seed)
{
    uint64_t sm_state = seed;
    rand32->state = (uint32_t)splitmix64_next(&sm_state);
    if (rand32->state == 0)
    {
        rand32->state = 2147483647;
    }
}

static inline uint32_t rand32_next(rand32_t *rand32)
{
    uint32_t x = rand32->state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rand32->state = x;
    return x;
}

/* ===============================================================
 X ORSHIFT 64 BITS*
 Periodo: 2^64 - 1
 =============================================================== */

typedef struct rand64
{
    uint64_t state;
} rand64_t;

static inline void rand64_seed(rand64_t *rand64, uint64_t seed)
{
    uint64_t sm_state = seed;
    rand64->state = splitmix64_next(&sm_state);
    if (rand64->state == 0)
    {
        rand64->state = 2305843009213693951ULL;
    }
}

static inline uint64_t rand64_next(rand64_t *rand64)
{
    uint64_t x = rand64->state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    rand64->state = x;
    return x;
}

/* ===============================================================
 X ORSHIFT 128+ (PLUS)
 Periodo: 2^128 - 1
 Nota: Retorna 64 bits por vez. O estado é que tem 128 bits.
 Esta é a versão melhorada que passa em mais testes estatísticos.
 =============================================================== */

typedef struct rand128
{
    uint64_t s[2];
} rand128_t;

static inline void rand128_seed(rand128_t *rand128, uint64_t seed)
{
    uint64_t sm_state = seed;
    rand128->s[0] = splitmix64_next(&sm_state);
    rand128->s[1] = splitmix64_next(&sm_state);

    if (rand128->s[0] == 0 && rand128->s[1] == 0)
    {
        rand128->s[0] = 1;
    }
}

static inline uint64_t rand128_next(rand128_t *rand128)
{
    uint64_t s1 = rand128->s[0];
    const uint64_t s0 = rand128->s[1];
    const uint64_t result = s1 + s0;

    rand128->s[0] = s0;
    s1 ^= s1 << 23;
    rand128->s[1] = s1 ^ s0 ^ (s1 >> 17) ^ (s0 >> 26);

    return result;
}

#endif
