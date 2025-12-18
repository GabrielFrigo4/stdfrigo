#include "stdrand.h"
#include <stdconst.h>
#include <stdhash.h>
#include <stdint.h>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#if defined(__GNUC__) || defined(__clang__)
#include <x86intrin.h>
#include <cpuid.h>
#else
#include <intrin.h>
#pragma intrinsic(_umul128)
#endif
#endif

/* ===============================================================
 * PRIMITIVAS INTERNAS (HELPERS)
 * ===============================================================
 * 1. SplitMix (Gerador de Sementes):
 * PRNGs modernos (como xoshiro) requerem um estado inicial
 * grande (128-256 bits) e estatisticamente "bem misturado".
 * O SplitMix é usado aqui para expandir uma única seed de 64 bits
 * nesse estado complexo, garantindo o "Efeito Avalanche" imediato
 * e prevenindo estados inválidos (como tudo zero).
 *
 * 2. Bitwise Rotation (Rotação de Bits):
 * Diferente do shift (<<) que descarta bits, a rotação move os
 * bits circularmente, preservando 100% da entropia.
 * Compiladores modernos otimizam isso para uma única instrução
 * de CPU (ROL/ROR).
 *
 * 3. Multiplicação portável 64x64=128 bits.
 * Retorna a parte BAIXA e armazena a ALTA
 * =============================================================== */

static inline uint64_t _stdrand_splitmix64_next_(uint64_t *state) {
    *state += PHI_INV_64;
    return hash64_int(*state);
}

static inline uint32_t _stdrand_rotl32_(const uint32_t x, int k) {
    return (x << k) | (x >> (32 - k));
}

static inline uint64_t _stdrand_rotl64_(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

static inline uint64_t _stdrand_mul128_(uint64_t x, uint64_t y, uint64_t *high) {
#if defined(__GNUC__) || defined(__clang__)
    unsigned __int128 r = (unsigned __int128)x * (unsigned __int128)y;
    *high = (uint64_t)(r >> 64);
    return (uint64_t)r;
#else
    return (uint64_t)_umul128(x, y, high);
#endif
}

/* ===============================================================
 * MACROS DE CONVERSÃO PARA PONTO FLUTUANTE (IEEE 754)
 * ===============================================================
 * Converte bits brutos em valores uniformemente distribuídos no
 * intervalo [0, 1).
 *
 * Método:
 * 1. Descarta os bits inferiores de baixa entropia (Shift Right).
 * 2. Mantém apenas os bits que cabem na mantissa (significand).
 * 3. Multiplica por 2^-N para normalizar para 0..1.
 *
 * Detalhes IEEE 754:
 * - float  (single): 23 bits de mantissa (+1 implícito) = 24 bits.
 * - double (double): 52 bits de mantissa (+1 implícito) = 53 bits.
 * =============================================================== */

#define RAND32_TO_FLOAT_UNIFORM(u) (((u) >> 8) * 0x1.0p-24f)
#define RAND64_TO_DOUBLE_UNIFORM(u) (((u) >> 11) * 0x1.0p-53)

/* ===============================================================
 * RAND32 (Algoritmo: xoshiro128**)
 * ===============================================================
 * Estado: 128 bits (Alta qualidade para 32-bit output)
 * Periodo: 2^128 - 1
 * =============================================================== */

rand32_t rand32_init(uint64_t seed) {
    rand32_t rng;
    uint64_t z1 = _stdrand_splitmix64_next_(&seed);
    rng.s[0] = (uint32_t)z1;
    rng.s[1] = (uint32_t)(z1 >> 32);
    uint64_t z2 = _stdrand_splitmix64_next_(&seed);
    rng.s[2] = (uint32_t)z2;
    rng.s[3] = (uint32_t)(z2 >> 32);
    if ((rng.s[0] | rng.s[1] | rng.s[2] | rng.s[3]) == 0) {
        rng.s[0] = PHI_INV_32;
    }
    return rng;
}

uint32_t rand32_next(rand32_t *rng) {
    const uint32_t result = _stdrand_rotl32_(rng->s[1] * 5, 7) * 9;
    const uint32_t t = rng->s[1] << 9;
    rng->s[2] ^= rng->s[0];
    rng->s[3] ^= rng->s[1];
    rng->s[1] ^= rng->s[2];
    rng->s[0] ^= rng->s[3];
    rng->s[2] ^= t;
    rng->s[3] = _stdrand_rotl32_(rng->s[3], 11);
    return result;
}

/* ===============================================================
 * RAND64 (Algoritmo: xoshiro256**)
 * ===============================================================
 * Estado: 256 bits
 * Periodo: 2^256 - 1
 * =============================================================== */

rand64_t rand64_init(uint64_t seed) {
    rand64_t rng;
    rng.s[0] = _stdrand_splitmix64_next_(&seed);
    rng.s[1] = _stdrand_splitmix64_next_(&seed);
    rng.s[2] = _stdrand_splitmix64_next_(&seed);
    rng.s[3] = _stdrand_splitmix64_next_(&seed);
    if ((rng.s[0] | rng.s[1] | rng.s[2] | rng.s[3]) == 0) {
        rng.s[0] = PHI_INV_64;
    }
    return rng;
}

uint64_t rand64_next(rand64_t *rng) {
    const uint64_t result = _stdrand_rotl64_(rng->s[1] * 5, 7) * 9;
    const uint64_t t = rng->s[1] << 17;
    rng->s[2] ^= rng->s[0];
    rng->s[3] ^= rng->s[1];
    rng->s[1] ^= rng->s[2];
    rng->s[0] ^= rng->s[3];
    rng->s[2] ^= t;
    rng->s[3] = _stdrand_rotl64_(rng->s[3], 45);
    return result;
}

/* ===============================================================
 * RAND FLOAT (xoshiro128+)
 * ===============================================================
 * Gera números de precisão simples [0, 1) diretamente.
 * =============================================================== */

rand_float_t rand_float_init(uint64_t seed) {
    rand_float_t rng;
    uint64_t z1 = _stdrand_splitmix64_next_(&seed);
    rng.s[0] = (uint32_t)z1;
    rng.s[1] = (uint32_t)(z1 >> 32);
    uint64_t z2 = _stdrand_splitmix64_next_(&seed);
    rng.s[2] = (uint32_t)z2;
    rng.s[3] = (uint32_t)(z2 >> 32);
    if ((rng.s[0] | rng.s[1] | rng.s[2] | rng.s[3]) == 0) {
        rng.s[0] = PHI_INV_32;
    }
    return rng;
}

float rand_float_next(rand_float_t *rng) {
    const uint32_t raw = rng->s[0] + rng->s[3];
    const uint32_t t = rng->s[1] << 9;
    rng->s[2] ^= rng->s[0];
    rng->s[3] ^= rng->s[1];
    rng->s[1] ^= rng->s[2];
    rng->s[0] ^= rng->s[3];
    rng->s[2] ^= t;
    rng->s[3] = _stdrand_rotl32_(rng->s[3], 11);
    return RAND32_TO_FLOAT_UNIFORM(raw);
}

/* ===============================================================
 * RAND DOUBLE (xoroshiro128+)
 * ===============================================================
 * Gera números de dupla precisão [0, 1) diretamente.
 * =============================================================== */

rand_double_t rand_double_init(uint64_t seed) {
    rand_double_t rng;
    rng.s[0] = _stdrand_splitmix64_next_(&seed);
    rng.s[1] = _stdrand_splitmix64_next_(&seed);
    if ((rng.s[0] | rng.s[1]) == 0) {
        rng.s[0] = PHI_INV_64;
    }
    return rng;
}

double rand_double_next(rand_double_t *rng) {
    const uint64_t s0 = rng->s[0];
    uint64_t s1 = rng->s[1];
    const uint64_t raw = s0 + s1;
    s1 ^= s0;
    rng->s[0] = _stdrand_rotl64_(s0, 24) ^ s1 ^ (s1 << 16);
    rng->s[1] = _stdrand_rotl64_(s1, 37);
    return RAND64_TO_DOUBLE_UNIFORM(raw);
}

/* ===============================================================
 * RE-SEED (Reinicialização Genérica)
 * ===============================================================
 * Permite reiniciar um gerador existente com uma nova seed.
 * Aqui o _Generic funciona pois passamos o ponteiro do RNG.
 * =============================================================== */

void rand32_seed(rand32_t *rng, uint64_t seed) {
    *rng = rand32_init(seed);
}

void rand64_seed(rand64_t *rng, uint64_t seed) {
    *rng = rand64_init(seed);
}

void rand_float_seed(rand_float_t *rng, uint64_t seed) {
    *rng = rand_float_init(seed);
}

void rand_double_seed(rand_double_t *rng, uint64_t seed) {
    *rng = rand_double_init(seed);
}

/* ===============================================================
 * FUNÇÕES DE SALTO (JUMP AHEAD)
 * ===============================================================
 * Uso: Paralelismo. Avança o estado como se tivesse chamado
 * next() 2^64 (rand32) ou 2^128 (rand64) vezes.
 * Cenário: Thread A usa o estado original. Thread B faz um jump
 * e usa o novo estado. Garantia matemática de não sobreposição.
 * =============================================================== */

void rand32_jump(rand32_t *rng) {
    static const uint32_t JUMP[] = {0x8764000bU, 0xf542d2d3U, 0x6fa035c3U, 0x77f2db5bU};
    uint32_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    for (int i = 0; i < 4; i++) {
        for (int b = 0; b < 32; b++) {
            if (JUMP[i] & (1U << b)) {
                s0 ^= rng->s[0];
                s1 ^= rng->s[1];
                s2 ^= rng->s[2];
                s3 ^= rng->s[3];
            }
            rand32_next(rng);
        }
    }
    rng->s[0] = s0;
    rng->s[1] = s1;
    rng->s[2] = s2;
    rng->s[3] = s3;
}

void rand64_jump(rand64_t *rng) {
    static const uint64_t JUMP[] = {
        0x180ec6d33cfd0abaULL, 0xd5a61266f0c9392cULL, 0xa9582618e03fc9aaULL, 0x39abdc4529b1661cULL};
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    for (int i = 0; i < 4; i++) {
        for (int b = 0; b < 64; b++) {
            if (JUMP[i] & (1ULL << b)) {
                s0 ^= rng->s[0];
                s1 ^= rng->s[1];
                s2 ^= rng->s[2];
                s3 ^= rng->s[3];
            }
            rand64_next(rng);
        }
    }
    rng->s[0] = s0;
    rng->s[1] = s1;
    rng->s[2] = s2;
    rng->s[3] = s3;
}

void rand_float_jump(rand_float_t *rng) {
    static const uint32_t JUMP[] = {0x8764000bU, 0xf542d2d3U, 0x6fa035c3U, 0x77f2db5bU};
    uint32_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    for (int i = 0; i < 4; i++) {
        for (int b = 0; b < 32; b++) {
            if (JUMP[i] & (1U << b)) {
                s0 ^= rng->s[0];
                s1 ^= rng->s[1];
                s2 ^= rng->s[2];
                s3 ^= rng->s[3];
            }
            rand_float_next(rng);
        }
    }
    rng->s[0] = s0;
    rng->s[1] = s1;
    rng->s[2] = s2;
    rng->s[3] = s3;
}

void rand_double_jump(rand_double_t *rng) {
    static const uint64_t JUMP[] = {0xdf900294d8f554a5ULL, 0x170865df4b3201fcULL};
    uint64_t s0 = 0, s1 = 0;
    for (int i = 0; i < 2; i++) {
        for (int b = 0; b < 64; b++) {
            if (JUMP[i] & (1ULL << b)) {
                s0 ^= rng->s[0];
                s1 ^= rng->s[1];
            }
            rand_double_next(rng);
        }
    }
    rng->s[0] = s0;
    rng->s[1] = s1;
}

/* ===============================================================
 * FUNÇÕES DE LIMITE [0, N)
 * ===============================================================
 * Algoritmo: Appleby/Lemire (Debiased Integer Multiplication)
 * Retorna: [0, limit)
 * =============================================================== */

uint32_t rand32_bound(rand32_t *rng, uint32_t limit) {
    uint32_t x = rand32_next(rng);
    uint64_t m = (uint64_t)x * (uint64_t)limit;
    uint32_t l = (uint32_t)m;
    if (l < limit) {
        uint32_t t = -limit % limit;
        while (l < t) {
            x = rand32_next(rng);
            m = (uint64_t)x * (uint64_t)limit;
            l = (uint32_t)m;
        }
    }
    return (uint32_t)(m >> 32);
}

uint64_t rand64_bound(rand64_t *rng, uint64_t limit) {
    uint64_t x = rand64_next(rng);
    uint64_t h;
    uint64_t l = _stdrand_mul128_(x, limit, &h);
    if (l < limit) {
        uint64_t t = -limit % limit;
        while (l < t) {
            x = rand64_next(rng);
            l = _stdrand_mul128_(x, limit, &h);
        }
    }
    return h;
}

float rand_float_bound(rand_float_t *rng, float limit) {
    return rand_float_next(rng) * limit;
}

double rand_double_bound(rand_double_t *rng, double limit) {
    return rand_double_next(rng) * limit;
}

/* ===============================================================
 * FUNÇÕES DE INTERVALO [Min, Max)
 * ===============================================================
 * Retorna: [min, max)
 * =============================================================== */

uint32_t rand32_range(rand32_t *rng, uint32_t min, uint32_t max) {
    return min + rand32_bound(rng, max - min);
}

uint64_t rand64_range(rand64_t *rng, uint64_t min, uint64_t max) {
    return min + rand64_bound(rng, max - min);
}

float rand_float_range(rand_float_t *rng, float min, float max) {
    return min + rand_float_next(rng) * (max - min);
}

double rand_double_range(rand_double_t *rng, double min, double max) {
    return min + rand_double_next(rng) * (max - min);
}

/* ===============================================================
 * HARDWARE RANDOM (x86-64 ONLY)
 * ===============================================================
 * Requer: CPU Intel (Ivy Bridge+) ou AMD (Zen+)
 * Flags: Em alguns compiladores antigos pode precisar de -mrdrnd
 * =============================================================== */

#if defined(__x86_64__) || defined(_M_X64)
#if defined(__GNUC__) || defined(__clang__)
#define __STDRAND_ATTR_RDRND__ __attribute__((target("rdrnd")))
#define __STDRAND_ATTR_RDSEED__ __attribute__((target("rdseed")))
#else
#define __STDRAND_ATTR_RDRND__
#define __STDRAND_ATTR_RDSEED__
#endif

/* ===============================================================
 * CPU FEATURE DETECTION (SAFEGUARDS)
 * ===============================================================
 * Verifica com segurança se a CPU suporta instruções modernas
 * antes de tentar executá-las.
 *
 * 1. Prevenção de Crash (SIGILL):
 * Executar RDRAND em CPUs antigas (Core 2 Duo, Pentium) causa
 * encerramento imediato do programa. Esta etapa evita isso.
 *
 * 2. Cache Estático (Lazy Loading):
 * O resultado do CPUID é armazenado em variável estática para
 * evitar o custo de verificar a cada chamada.
 * =============================================================== */

static inline bool _stdrand_has_rdrand_(void) {
    static int has_feature = -1;
    if (has_feature != -1)
        return has_feature == 1;
    int info[4] = {0};
#if defined(__GNUC__) || defined(__clang__)
    __cpuid(1, info[0], info[1], info[2], info[3]);
#else
    __cpuid(info, 1);
#endif
    has_feature = (info[2] & (1 << 30)) ? 1 : 0;
    return _stdrand_has_rdrand_();
}

static inline bool _stdrand_has_rdseed_(void) {
    static int has_feature = -1;
    if (has_feature != -1)
        return has_feature == 1;
    int info[4] = {0};
#if defined(__GNUC__) || defined(__clang__)
    __cpuid(0, info[0], info[1], info[2], info[3]);
#else
    __cpuid(info, 0);
#endif
    if (info[0] < 7) {
        has_feature = 0;
        return false;
    }
#if defined(__GNUC__) || defined(__clang__)
    __cpuid_count(7, 0, info[0], info[1], info[2], info[3]);
#else
    __cpuidex(info, 7, 0);
#endif
    has_feature = (info[1] & (1 << 18)) ? 1 : 0;
    return _stdrand_has_rdseed_();
}

/* ===============================================================
 * RDRAND (Pseudo-Aleatório de Hardware / CSPRNG)
 * ===============================================================
 * Gera números via hardware (AES-CTR), re-semeado periodicamente
 * pela entropia térmica do processador.
 * Vantagem: Extremamente rápido e estatisticamente perfeito.
 * =============================================================== */

__STDRAND_ATTR_RDRND__
bool rand32_hw_fast(uint32_t *out) {
    if (!_stdrand_has_rdrand_())
        return false;
    for (int i = 0; i < 0xff; i++) {
        if (_rdrand32_step(out))
            return true;
        _mm_pause();
    }
    return false;
}

__STDRAND_ATTR_RDRND__
bool rand64_hw_fast(uint64_t *out) {
    if (!_stdrand_has_rdrand_())
        return false;
    for (int i = 0; i < 0xff; i++) {
        if (_rdrand64_step(out))
            return true;
        _mm_pause();
    }
    return false;
}

/* ===============================================================
 * RDRAND (Pseudo-Aleatório de Hardware / CSPRNG)
 * ===============================================================
 * Gera números via hardware (AES-CTR), re-semeado periodicamente
 * pela entropia térmica do processador.
 * Vantagem: Extremamente rápido e estatisticamente perfeito.
 * =============================================================== */

__STDRAND_ATTR_RDSEED__
bool rand32_hw_entropy(uint32_t *out) {
    if (!_stdrand_has_rdseed_())
        return false;
    for (int i = 0; i < 0xfff; i++) {
        if (_rdseed32_step(out))
            return true;
        _mm_pause();
    }
    return false;
}

__STDRAND_ATTR_RDSEED__
bool rand64_hw_entropy(uint64_t *out) {
    if (!_stdrand_has_rdseed_())
        return false;
    for (int i = 0; i < 0xfff; i++) {
        if (_rdseed64_step(out))
            return true;
        _mm_pause();
    }
    return false;
}

/* ===============================================================
 * AUTO SEED (Helper)
 * ===============================================================
 * Tenta obter uma seed de entropia pura (RDSEED). Se falhar usa o
 * contador de ciclos da CPU (RDTSC) como fallback.
 * =============================================================== */

uint32_t rand32_hw_seed(void) {
    uint32_t seed;
    if (rand32_hw_entropy(&seed))
        return seed;
    if (rand32_hw_fast(&seed))
        return seed;
    return (uint32_t)hash64_int((uint64_t)__rdtsc());
}

uint64_t rand64_hw_seed(void) {
    uint64_t seed;
    if (rand64_hw_entropy(&seed))
        return seed;
    if (rand64_hw_fast(&seed))
        return seed;
    return hash64_int((uint64_t)__rdtsc());
}
#endif
