#include "stdhash.h"
#include <string.h>
#include <stdconst.h>

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
 * 1. Multiplicação portável 64x64=128 bits.
 * Retorna a parte BAIXA e armazena a ALTA.
 *
 * 2. Leituras Seguras (Unaligned).
 * Necessárias para o algoritmo WyHash processar buffers
 * sem violar regras de alinhamento.
 * =============================================================== */

static inline uint64_t _stdhash_mul128_(uint64_t x, uint64_t y, uint64_t *high) {
#if defined(__GNUC__) || defined(__clang__)
    unsigned __int128 r = (unsigned __int128)x * (unsigned __int128)y;
    *high = (uint64_t)(r >> 64);
    return (uint64_t)r;
#else
    return (uint64_t)_umul128(x, y, high);
#endif
}

static inline uint64_t _stdhash_read64_(const uint8_t *p) {
    uint64_t v;
    memcpy(&v, p, sizeof(uint64_t));
    return v;
}

static inline uint64_t _stdhash_read_small_(const uint8_t *p, size_t k) {
    uint64_t v = 0;
    if (k >= 4) {
        memcpy(&v, p, 4);
        v <<= 32;
        p += 4;
        k -= 4;
    }
    if (k >= 2) {
        uint16_t t;
        memcpy(&t, p, 2);
        v |= (uint64_t)t << 16;
        p += 2;
        k -= 2;
    }
    if (k >= 1) {
        v |= *p;
    }
    return v;
}

/* ===============================================================
 * INTEGER HASH MIXERS
 * ===============================================================
 * Algoritmos de mistura de bits O(1) estatisticamente superiores.
 * 32-bit: "lowbias32" (Chris Wellons).
 * 64-bit: "Stafford Mix 13".
 * =============================================================== */

uint32_t hash32_int(uint32_t k) {
    const uint32_t c1 = 0x7feb352dU;
    const uint32_t c2 = 0x846ca68bU;

    k ^= k >> 16;
    k *= c1;
    k ^= k >> 15;
    k *= c2;
    k ^= k >> 16;
    return k;
}

uint64_t hash64_int(uint64_t k) {
    const uint64_t c1 = 0xbf58476d1ce4e5b9ULL;
    const uint64_t c2 = 0x94d049bb133111ebULL;

    k ^= k >> 30;
    k *= c1;
    k ^= k >> 27;
    k *= c2;
    k ^= k >> 31;
    return k;
}

/* ===============================================================
 * FAST HASH (WyHash Variant)
 * ===============================================================
 * O novo "Padrão Ouro" para hashing de software.
 * Substitui FNV-1a e Jenkins.
 * Vantagem: Usa matemática de 128 bits e leituras de 64 bits.
 * Extremamente rápido e passa no SMHasher.
 * =============================================================== */

uint64_t hash64_fast(const void *buff, size_t len) {
    const uint8_t *p = (const uint8_t *)buff;
    uint64_t seed = PHI_INV_64 ^ len;
    uint64_t see1 = seed;
    uint64_t high;
    uint64_t low;

    const uint64_t _wyp0 = 0xa0761d6478bd642fULL;
    const uint64_t _wyp1 = 0xe7037ed1a0b428dbULL;
    const uint64_t _wyp2 = 0x8ebc6af09c88c6e3ULL;
    const uint64_t _wyp3 = 0x589965cc75374cc3ULL;
    const uint64_t _wyp4 = 0x1d8e4e27c47d124fULL;

    while (len >= 16) {
        uint64_t v1 = _stdhash_read64_(p);
        uint64_t v2 = _stdhash_read64_(p + 8);

        low = _stdhash_mul128_(seed ^ v1 ^ _wyp0, _wyp1, &high);
        seed = low ^ high;

        low = _stdhash_mul128_(see1 ^ v2 ^ _wyp2, _wyp3, &high);
        see1 = low ^ high;

        p += 16;
        len -= 16;
    }

    if (len >= 8) {
        uint64_t v1 = _stdhash_read64_(p);
        uint64_t v2 = _stdhash_read64_(p + len - 8);

        low = _stdhash_mul128_(seed ^ v1 ^ _wyp0, _wyp1, &high);
        seed = low ^ high;

        low = _stdhash_mul128_(see1 ^ v2 ^ _wyp2, _wyp3, &high);
        see1 = low ^ high;
    } else if (len > 0) {
        uint64_t v1 = _stdhash_read_small_(p, len);
        low = _stdhash_mul128_(seed ^ v1 ^ _wyp0, _wyp1, &high);
        seed = low ^ high;
    }

    low = _stdhash_mul128_(seed, _wyp1, &high);
    uint64_t a = low ^ high;

    low = _stdhash_mul128_(see1, _wyp1, &high);
    uint64_t b = low ^ high;

    low = _stdhash_mul128_(a ^ b, _wyp4, &high);
    return low ^ high;
}

uint32_t hash32_fast(const void *buff, size_t len) {
    return (uint32_t)hash64_fast(buff, len);
}

/* ===============================================================
 * AUXILIAR: COMBINE HASH
 * ===============================================================
 * Combina uma seed existente com um novo valor.
 * =============================================================== */

uint32_t hash32_combine(uint32_t seed, uint32_t next_hash) {
    return seed ^ (next_hash + PHI_INV_32 + (seed << 6) + (seed >> 2));
}

uint64_t hash64_combine(uint64_t seed, uint64_t next_hash) {
    return seed ^ (next_hash + PHI_INV_64 + (seed << 6) + (seed >> 2));
}

/* ===============================================================
 * HARDWARE HASHING (X86-64 ONLY)
 * ===============================================================
 * Algoritmo: CRC32-C (Castagnoli).
 * Requer: CPU com suporte a SSE4.2.
 * =============================================================== */

#if defined(__x86_64__) || defined(_M_X64)
#if defined(__GNUC__) || defined(__clang__)
#define _STDHASH_ATTR_SSE42_ __attribute__((target("sse4.2")))
#else
#define _STDHASH_ATTR_SSE42_
#endif

/* ===============================================================
 * CPU FEATURE DETECTION (SAFEGUARDS)
 * ===============================================================
 * Verifica com segurança se a CPU suporta instruções modernas
 * antes de tentar executá-las.
 *
 * 1. Prevenção de Crash (SIGILL):
 * Tentar executar instruções CRC32 (_mm_crc32_u64) em CPUs
 * antigas (anteriores a Nehalem/Bulldozer) causa encerramento
 * imediato do programa. Esta etapa evita isso.
 *
 * 2. Cache Estático (Lazy Loading):
 * O resultado do CPUID é armazenado em variável estática para
 * evitar o custo de verificar a cada chamada.
 * =============================================================== */

static bool _stdhash_has_sse42_(void) {
    static int has_feature = -1;
    if (has_feature != -1) {
        return has_feature == 1;
    }

    int info[4] = {0};
#if defined(__GNUC__) || defined(__clang__)
    __cpuid(1, info[0], info[1], info[2], info[3]);
#else
    __cpuid(info, 1);
#endif
    has_feature = (info[2] & (1 << 20)) ? 1 : 0;

    return _stdhash_has_sse42_();
}

/* ===============================================================
 * HARDWARE HASHING (CRC32-C)
 * ===============================================================
 * Algoritmo: CRC32 via Hardware (Polinômio Castagnoli).
 * Vantagem: Atinge GB/s processando buffers diretamente
 * no pipeline da CPU. Ideal para checksums e HashMaps.
 * Nota: A versão 64-bit usa 3 fluxos paralelos para
 * maximizar o throughput e esconder latência de instrução.
 * =============================================================== */

_STDHASH_ATTR_SSE42_
bool hash32_hw(const void *buff, size_t len, uint32_t *out) {
    if (!_stdhash_has_sse42_()) {
        return false;
    }

    const uint8_t *tail = (const uint8_t *)buff;
    const uint8_t *end_64 = tail + (len & ~7);
    uint32_t hash = PHI_INV_32;

    while (tail < end_64) {
        uint64_t k;
        memcpy(&k, tail, sizeof(uint64_t));
        hash = (uint32_t)_mm_crc32_u64(hash, k);
        tail += sizeof(uint64_t);
    }

    size_t remaining = len & 7;
    while (remaining > 0) {
        if (remaining >= 4) {
            uint32_t k;
            memcpy(&k, tail, 4);
            hash = _mm_crc32_u32(hash, k);
            tail += 4;
            remaining -= 4;
        } else if (remaining >= 2) {
            uint16_t k;
            memcpy(&k, tail, 2);
            hash = _mm_crc32_u16(hash, k);
            tail += 2;
            remaining -= 2;
        } else {
            hash = _mm_crc32_u8(hash, *tail);
            tail += 1;
            remaining -= 1;
        }
    }
    *out = hash;
    return true;
}

_STDHASH_ATTR_SSE42_
bool hash64_hw(const void *buff, size_t len, uint64_t *out) {
    if (!_stdhash_has_sse42_()) {
        return false;
    }

    const uint8_t *tail = (const uint8_t *)buff;
    const uint8_t *end_64 = tail + (len & ~7);

    uint64_t h1 = (uint32_t)(PHI_INV_64 >> 32);
    uint64_t h2 = (uint32_t)PHI_INV_64;

    while (tail < end_64) {
        uint64_t k;
        memcpy(&k, tail, sizeof(uint64_t));
        h1 = _mm_crc32_u64(h1, k);
        h2 = _mm_crc32_u64(h2, k ^ 0xffffffffffffffffULL);
        tail += sizeof(uint64_t);
    }

    size_t remaining = len & 7;
    while (remaining > 0) {
        uint8_t k = *tail++;
        h1 = _mm_crc32_u8((uint32_t)h1, k);
        remaining--;
    }
    *out = (h1 << 32) | (uint32_t)h2;
    return true;
}
#endif
