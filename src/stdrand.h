#ifndef STDRAND_H
#define STDRAND_H

#include <stdint.h>
#include <stdbool.h>
#include <stdconst.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* ===============================================================
     * ALGORITMOS AUXILIARES (SPLITMIX)
     * Uso: Inicializar os estados de forma segura.
     * =============================================================== */

    static inline uint32_t __stdrand_splitmix32_next__(uint32_t *state)
    {
        uint32_t z = (*state += PHI_INV_32);
        z = (z ^ (z >> 16)) * 0x85ebca6bU;
        z = (z ^ (z >> 13)) * 0xc2b2ae35U;
        return z ^ (z >> 16);
    }

    static inline uint64_t __stdrand_splitmix64_next__(uint64_t *state)
    {
        uint64_t z = (*state += PHI_INV_64);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }

    /* ===============================================================
     * XORSHIFT 32 BITS
     * Periodo: 2^32 - 1
     * =============================================================== */

    typedef struct rand32
    {
        uint32_t state;
    } rand32_t;

    static inline rand32_t rand32_init(uint32_t seed)
    {
        rand32_t rand32;
        rand32.state = __stdrand_splitmix32_next__(&seed);
        if (rand32.state == 0)
        {
            rand32.state = PHI_INV_32;
        }
        return rand32;
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
     * XORSHIFT 64 BITS
     * Periodo: 2^64 - 1
     * =============================================================== */

    typedef struct rand64
    {
        uint64_t state;
    } rand64_t;

    static inline rand64_t rand64_init(uint64_t seed)
    {
        rand64_t rand64;
        rand64.state = __stdrand_splitmix64_next__(&seed);
        if (rand64.state == 0)
        {
            rand64.state = PHI_INV_64;
        }
        return rand64;
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
     * XORSHIFT 128+ (PLUS)
     * Periodo: 2^128 - 1
     * Nota: Retorna 64 bits por vez. O estado é que tem 128 bits.
     * Esta é a versão melhorada que passa em mais testes estatísticos.
     * =============================================================== */

    typedef struct rand128
    {
        uint64_t s[2];
    } rand128_t;

    static inline rand128_t rand128_init(uint64_t seed)
    {
        rand128_t rand128;
        rand128.s[0] = __stdrand_splitmix64_next__(&seed);
        rand128.s[1] = __stdrand_splitmix64_next__(&seed);
        if (rand128.s[0] == 0)
        {
            rand128.s[0] = PHI_INV_64;
        }
        if (rand128.s[1] == 0)
        {
            rand128.s[1] = PHI_INV_64;
        }
        return rand128;
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

    /* ===============================================================
     * MACROS DE CONVERSÃO PARA PONTO FLUTUANTE
     * Uso: Converte inteiros aleatórios para o intervalo [0, 1).
     * Nota: A lógica consiste em manter apenas os bits da mantissa (53 ou 24)
     * e multiplicar pelo inverso da potência de 2 correspondente.
     * =============================================================== */

#define TO_DBL_01(u) \
    (((u) >> 11) * 0x1.0p-53)

#define TO_FLT_01(u) \
    (((u) >> 8) * 0x1.0p-24f)

    /* ===============================================================
     * HARDWARE RANDOM (x86-64 ONLY)
     * Requer: CPU Intel (Ivy Bridge+) ou AMD (Zen+)
     * Flags: Em alguns compiladores antigos pode precisar de -mrdrnd
     * =============================================================== */

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#if defined(__GNUC__) || defined(__clang__)
#include <x86intrin.h>
#else
#include <intrin.h>
#endif

#if defined(__GNUC__) || defined(__clang__)
#define __STDRAND_ATTR_RDRND__ __attribute__((target("rdrnd")))
#define __STDRAND_ATTR_RDSEED__ __attribute__((target("rdseed")))
#else
#define __STDRAND_ATTR_RDRND__
#define __STDRAND_ATTR_RDSEED__
#endif

    /* ===============================================================
     * RDRAND (Pseudo-Aleatório de Hardware / CSPRNG)
     * Gera números usando o gerador determinístico do hardware (AES-CTR),
     * que é re-semeado periodicamente pela entropia térmica.
     * Vantagem: Muito rápido, estatisticamente perfeito.
     * =============================================================== */

#define __STDRAND_RDRAND_LIMIT__ 0xff

    __STDRAND_ATTR_RDRND__
    static inline bool rand32_hw_fast(uint32_t *out)
    {
        for (int i = 0; i < __STDRAND_RDRAND_LIMIT__; i++)
        {
            if (_rdrand32_step(out))
            {
                return true;
            }
            _mm_pause();
        }
        return false;
    }

    __STDRAND_ATTR_RDRND__
    static inline bool rand64_hw_fast(uint64_t *out)
    {
        for (int i = 0; i < __STDRAND_RDRAND_LIMIT__; i++)
        {
            if (_rdrand64_step(out))
            {
                return true;
            }
            _mm_pause();
        }
        return false;
    }

    /* ===============================================================
     * RDSEED (Aleatório Real / Entropia Pura)
     * Gera números diretamente do ruído térmico do hardware.
     * Vantagem: Não determinístico (True Random).
     * Uso: Ideal para gerar SEEDS para o rand64_seed().
     * Nota: Pode ser mais lento que o RDRAND.
     * =============================================================== */

#define __STDRAND_RDSEED_LIMIT__ 0xfff

    __STDRAND_ATTR_RDSEED__
    static inline bool rand32_hw_entropy(uint32_t *out)
    {
        for (int i = 0; i < __STDRAND_RDSEED_LIMIT__; i++)
        {
            if (_rdseed32_step(out))
            {
                return true;
            }
            _mm_pause();
        }
        return false;
    }

    __STDRAND_ATTR_RDSEED__
    static inline bool rand64_hw_entropy(uint64_t *out)
    {
        for (int i = 0; i < __STDRAND_RDSEED_LIMIT__; i++)
        {
            if (_rdseed64_step(out))
            {
                return true;
            }
            _mm_pause();
        }
        return false;
    }

    /* ===============================================================
     * AUTO SEED (Helper)
     * Tenta obter uma seed de entropia pura (RDSEED). Se falhar usa o
     * contador de ciclos da CPU (RDTSC) como fallback.
     * Uso: rand64_seed(&rng, rand64_hw_seed());
     * =============================================================== */

    static inline uint32_t rand32_hw_seed(void)
    {
        uint32_t seed = (uint32_t)__rdtsc();
        if (rand32_hw_entropy(&seed))
        {
            return seed;
        }
        if (rand32_hw_fast(&seed))
        {
            return seed;
        }
        return __stdrand_splitmix32_next__(&seed);
    }

    static inline uint64_t rand64_hw_seed(void)
    {
        uint64_t seed = (uint64_t)__rdtsc();
        if (rand64_hw_entropy(&seed))
        {
            return seed;
        }
        if (rand64_hw_fast(&seed))
        {
            return seed;
        }
        return __stdrand_splitmix64_next__(&seed);
    }
#endif

#ifdef __cplusplus
}
#endif

#endif
