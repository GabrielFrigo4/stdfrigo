#ifndef STDHASH_H
#define STDHASH_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdconst.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* ===============================================================
     * MURMURHASH3 INTEGER FINALIZER
     * Uso: Hashing de inteiros (IDs, Ponteiros) para índices.
     * Vantagem: O(1) sem loops. Transforma um padrão sequencial
     * (1, 2, 3...) em um padrão aleatório uniforme.
     * =============================================================== */

#define __STDHASH_MURMUR3_32_C1__ 0x85ebca6bU
#define __STDHASH_MURMUR3_32_C2__ 0xc2b2ae35U
#define __STDHASH_MURMUR3_64_C1__ 0xff51afd7ed558ccdULL
#define __STDHASH_MURMUR3_64_C2__ 0xc4ceb9fe1a85ec53ULL

    static inline uint32_t hash32_u32(uint32_t k)
    {
        k ^= k >> 16;
        k *= __STDHASH_MURMUR3_32_C1__;
        k ^= k >> 13;
        k *= __STDHASH_MURMUR3_32_C2__;
        k ^= k >> 16;
        return k;
    }

    static inline uint64_t hash64_u64(uint64_t k)
    {
        k ^= k >> 33;
        k *= __STDHASH_MURMUR3_64_C1__;
        k ^= k >> 33;
        k *= __STDHASH_MURMUR3_64_C2__;
        k ^= k >> 33;
        return k;
    }

    /* ===============================================================
     *  HASH 32 FROM 64 (Helper)
     *  Uso: Quando você tem uma chave de 64 bits (ex: ponteiro) mas
     *  precisa de um hash de 32 bits.
     *  =============================================================== */

    static inline uint32_t hash32_u64(uint64_t k)
    {
        return hash32_u32((uint32_t)(k ^ (k >> 32)));
    }

    /* ===============================================================
     * JENKINS ONE-AT-A-TIME
     * Uso: Tabelas hash genéricas, dados binários variados.
     * Vantagem: Efeito avalanche muito forte (cada bit de entrada
     * afeta cerca de 50% dos bits de saída).
     * =============================================================== */

    static inline uint32_t hash32_buff(const void *buff, size_t len)
    {
        const uint8_t *bytes = (const uint8_t *)buff;
        uint32_t hash = 0;
        for (size_t i = 0; i < len; i++)
        {
            hash += bytes[i];
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }
        return hash32_u32(hash);
    }

    static inline uint64_t hash64_buff(const void *buff, size_t len)
    {
        const uint8_t *bytes = (const uint8_t *)buff;
        uint64_t hash = 0;
        for (size_t i = 0; i < len; i++)
        {
            hash += bytes[i];
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }
        return hash64_u64(hash);
    }

    /* ===============================================================
     FNV-1a
     Uso: Strings curtas, chaves de texto, caminhos de arquivo.
     Vantagem: Implementação trivial e boa dispersão para textos.
     =============================================================== */

#define __STDHASH_FNV1A_PRIME_32__ 16777619U
#define __STDHASH_FNV1A_OFFSET_32__ 2166136261U
#define __STDHASH_FNV1A_PRIME_64__ 1099511628211ULL
#define __STDHASH_FNV1A_OFFSET_64__ 14695981039346656037ULL

    static inline uint32_t fasthash32_buff(const void *buff, size_t len)
    {
        const uint8_t *bytes = (const uint8_t *)buff;
        uint32_t hash = __STDHASH_FNV1A_OFFSET_32__;
        for (size_t i = 0; i < len; i++)
        {
            hash ^= bytes[i];
            hash *= __STDHASH_FNV1A_PRIME_32__;
        }
        return hash;
    }

    static inline uint64_t fasthash64_buff(const void *buff, size_t len)
    {
        const uint8_t *bytes = (const uint8_t *)buff;
        uint64_t hash = __STDHASH_FNV1A_OFFSET_64__;
        for (size_t i = 0; i < len; i++)
        {
            hash ^= bytes[i];
            hash *= __STDHASH_FNV1A_PRIME_64__;
        }
        return hash;
    }

    /* ===============================================================
     AUXILIAR: COMBINE HASH (Inspirado no Boost)
     Uso: Quando você precisa combinar dois hashes (ex: hash de
     Coordenada X + hash de Coordenada Y).
     Nota1: O "magic number" 0x9e3779b9U é uma aproximação
     da Razão Áurea (phi) inversa escalada para 32 bits. Isso ajuda
     a evitar simetrias e colisões em dados estruturados.
     Nota2: O "magic number" 0x9e3779b97f4a7c15ULL é uma aproximação
     da Razão Áurea (phi) inversa escalada para 64 bits. Isso ajuda
     a evitar simetrias e colisões em dados estruturados.
     =============================================================== */

    static inline uint32_t hash32_combine(uint32_t seed, uint32_t next_hash)
    {
        return seed ^ (next_hash + PHI_INV_32 + (seed << 6) + (seed >> 2));
    }

    static inline uint64_t hash64_combine(uint64_t seed, uint64_t next_hash)
    {
        return seed ^ (next_hash + PHI_INV_64 + (seed << 6) + (seed >> 2));
    }

    /* ===============================================================
     HARDWARE HASHING (X86-64 ONLY)
     Algoritmo: CRC32-C (Castagnoli)
     Requer: CPU com suporte a SSE4.2 (Intel Nehalem+ / AMD Bulldozer+)
     Uso: O algoritmo de hash mais rápido possível na arquitetura x86.
     =============================================================== */

#if defined(__x86_64__) || defined(_M_X64)

#include <nmmintrin.h>

#if defined(__GNUC__) || defined(__clang__)
#define __STDHASH_ATTR_SSE42__ __attribute__((target("sse4.2")))
#else
#define __STDHASH_ATTR_SSE42__
#endif

    __STDHASH_ATTR_SSE42__
    static inline uint32_t hash32_hw(const void *buff, size_t len)
    {
        const uint8_t *tail = (const uint8_t *)buff;
        const uint8_t *end_64 = tail + (len & ~7);
        uint32_t hash = PHI_INV_32;

        while (tail < end_64)
        {
            uint64_t k;
            memcpy(&k, tail, sizeof(uint64_t));

            hash = (uint32_t)_mm_crc32_u64(hash, k);
            tail += sizeof(uint64_t);
        }

        size_t remaining = len & 7;
        while (remaining > 0)
        {
            if (remaining >= sizeof(uint32_t))
            {
                uint32_t k;
                memcpy(&k, tail, sizeof(uint32_t));
                hash = _mm_crc32_u32(hash, k);
                tail += sizeof(uint32_t);
                remaining -= sizeof(uint32_t);
            }
            else if (remaining >= sizeof(uint16_t))
            {
                uint16_t k;
                memcpy(&k, tail, sizeof(uint16_t));
                hash = _mm_crc32_u16(hash, k);
                tail += sizeof(uint16_t);
                remaining -= sizeof(uint16_t);
            }
            else
            {
                hash = _mm_crc32_u8(hash, *tail);
                tail += sizeof(uint8_t);
                remaining -= sizeof(uint8_t);
            }
        }

        return hash;
    }

    __STDHASH_ATTR_SSE42__
    static inline uint64_t hash64_hw(const void *buff, size_t len)
    {
        const uint8_t *tail = (const uint8_t *)buff;
        const uint8_t *end_64 = tail + (len & ~7);

        uint64_t h1 = (uint32_t)(PHI_INV_64 >> 32);
        uint64_t h2 = (uint32_t)PHI_INV_64;

        while (tail < end_64)
        {
            uint64_t k;
            memcpy(&k, tail, sizeof(uint64_t));

            h1 = _mm_crc32_u64(h1, k);
            h2 = _mm_crc32_u64(h2, k ^ 0xffffffffffffffffULL);
            tail += sizeof(uint64_t);
        }

        size_t remaining = len & 7;
        while (remaining > 0)
        {
            if (remaining >= sizeof(uint32_t))
            {
                uint32_t k;
                memcpy(&k, tail, sizeof(uint32_t));

                h1 = _mm_crc32_u32((uint32_t)h1, k);
                h2 = _mm_crc32_u32((uint32_t)h2, k ^ 0xffffffffU);
                tail += sizeof(uint32_t);
                remaining -= sizeof(uint32_t);
            }
            else if (remaining >= sizeof(uint16_t))
            {
                uint16_t k;
                memcpy(&k, tail, sizeof(uint16_t));

                h1 = _mm_crc32_u16((uint32_t)h1, k);
                h2 = _mm_crc32_u16((uint32_t)h2, (uint16_t)(k ^ 0xffff));
                tail += sizeof(uint16_t);
                remaining -= sizeof(uint16_t);
            }
            else
            {
                uint8_t k = *tail;
                h1 = _mm_crc32_u8((uint32_t)h1, k);
                h2 = _mm_crc32_u8((uint32_t)h2, (uint8_t)(k ^ 0xff));
                tail += sizeof(uint8_t);
                remaining -= sizeof(uint8_t);
            }
        }

        return (h1 << 32) | (uint32_t)h2;
    }

#endif

#ifdef __cplusplus
}
#endif

#endif
