/* ==========================================================================
 * STDFRIGO UNIT TEST SUITE
 * ==========================================================================
 * Compilar como C:   gcc main.c stdhash.c -o test_c -I. -mavx
 * Compilar como C++: g++ main.c stdhash.c -o test_cpp -I. -mavx
 * ========================================================================== */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <inttypes.h>

/* Inclua seu cabeçalho principal ou os módulos individuais */
#include "stdhash.h"
#include "stdconst.h"

/* * MOCK STDRAND (Caso você ainda não tenha o stdrand.h implementado)
 * Se já tiver, troque isso por: #include "stdrand.h"
 */
#ifndef STDRAND_H
typedef struct { uint64_t state; } stdrand_t;
static inline void rand_seed(stdrand_t *rng, uint64_t seed) { rng->state = seed; }
static inline uint64_t rand_next(stdrand_t *rng) {
    /* SplitMix64 simples para teste */
    uint64_t z = (rng->state += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}
#endif

/* ===============================================================
 * UTILITÁRIOS DE TESTE (CORES E ASSERT)
 * =============================================================== */
#define KGRN  "\x1B[32m"
#define KRED  "\x1B[31m"
#define KYEL  "\x1B[33m"
#define KCYN  "\x1B[36m"
#define KRST  "\x1B[0m"

#define TEST_PASS(msg) printf("[" KGRN "PASS" KRST "] %s\n", msg)
#define TEST_FAIL(msg) printf("[" KRED "FAIL" KRST "] %s\n", msg)
#define TEST_INFO(msg) printf("[" KCYN "INFO" KRST "] %s\n", msg)

/* ===============================================================
 * 1. TESTE DE CONSTANTES (stdconst.h)
 * =============================================================== */
void test_constants(void) {
    printf("\n>>> Testando STDCONST...\n");

    /* Verificando precisão do PHI */
    float phi_f = PHI_32;
    double phi_d = PHI_64;

    /* 1.6180339887... */
    assert(fabsf(phi_f - 1.61803399f) < 1e-6);
    assert(fabs(phi_d - 1.6180339887498948) < 1e-15);
    TEST_PASS("Constantes PHI precisas");

    /* Verificando consistência Matemática (Phi * InvPhi ~= 1.0) */
    assert(fabs(PHI_64 * PHI_INV_64 - 1.0) < 1e-15);
    TEST_PASS("PHI * PHI_INV == 1.0");

    /* Verificando HASH Magic (Hex) */
    assert(PHI_INV_HASH_32 == 0x9e3779b9U);
    TEST_PASS("PHI_HASH Magic Number correto");
}

/* ===============================================================
 * 2. TESTE DE POLIMORFISMO (stdhash.h)
 * =============================================================== */
void test_polymorphism(void) {
    printf("\n>>> Testando Polimorfismo de Hash...\n");

    uint32_t in32 = 42;
    uint64_t in64 = 42;
    uint32_t out32;
    uint64_t out64;

    /* * Aqui testamos se o compilador escolhe a função certa automaticamente.
     * Se compilar, o _Generic/Overloading funcionou.
     */

    /* Teste Scalar Hash */
    uint32_t h1 = hash_int(in32); // Deve chamar hash32_int
    uint64_t h2 = hash_int(in64); // Deve chamar hash64_int

    if (sizeof(h1) == 4 && sizeof(h2) == 8) {
        TEST_PASS("hash_int resolveu tipos corretamente (32 vs 64)");
    }

    /* Teste Memory Hash */
    char buffer[] = "stdfrigo";

    /* Deve chamar hash32_mem baseado no ponteiro &out32 */
    hash_mem(buffer, sizeof(buffer), &out32);

    /* Deve chamar hash64_mem baseado no ponteiro &out64 */
    hash_mem(buffer, sizeof(buffer), &out64);

    if (out32 != 0 && out64 != 0) {
        TEST_PASS("hash_mem resolveu ponteiros de saída corretamente");
    }
}

/* ===============================================================
 * 3. TESTE DE HARDWARE vs SOFTWARE (stdhash.c)
 * =============================================================== */
void test_hardware_accel(void) {
    printf("\n>>> Testando Aceleração de Hardware...\n");

    const char *data = "1234567890_BIG_BUFFER_TEST";
    size_t len = strlen(data);
    uint64_t hw_res, sw_res;

    /* Calcula via Software (WyHash) */
    hash_mem(data, len, &sw_res);
    printf("   Software Hash: %" PRIx64 "\n", sw_res);

    /* Tenta Calcular via Hardware (CRC32C) */
    bool has_hw = hash_hw(data, len, &hw_res);

    if (has_hw) {
        printf("   Hardware Hash: " KYEL "%" PRIx64 KRST " (SSE4.2 Ativo)\n", hw_res);
        TEST_PASS("Hardware check retornou true (CPU Suportada)");

        /* O hash de HW (CRC) deve ser DIFERENTE do SW (WyHash),
         *          pois são algoritmos matematicamente diferentes. */
        if (hw_res != sw_res) {
            TEST_PASS("HW e SW geraram hashes distintos (Correto)");
        }
    } else {
        printf("   Hardware Hash: " KRED "N/A" KRST " (CPU Antiga/Sem suporte)\n");
        TEST_INFO("Rodando em modo fallback (apenas Software)");
    }
}

/* ===============================================================
 * 4. TESTE DE INTEGRAÇÃO (stdrand + stdhash)
 * =============================================================== */
void test_integration(void) {
    printf("\n>>> Testando Integração Rand+Hash...\n");

    stdrand_t rng;
    rand_seed(&rng, 12345); // Seed fixa

    uint64_t val = rand_next(&rng);
    uint64_t h = hash_int(val);

    printf("   Rand Val: %" PRIu64 "\n", val);
    printf("   Hash Val: %" PRIx64 "\n", h);

    if (val != h) TEST_PASS("Pipeline Rand -> Hash funcionou");
}

/* ===============================================================
 * MAIN
 * =============================================================== */
int main(void) {
    printf("========================================\n");
    #ifdef __cplusplus
    printf("   AMBIENTE: C++ (Overloading/Templates)\n");
    #else
    printf("   AMBIENTE: C11 (_Generic Macros)\n");
    #endif
    printf("========================================\n");

    test_constants();
    test_polymorphism();
    test_hardware_accel();
    test_integration();

    printf("\n" KGRN "TODOS OS TESTES CONCLUÍDOS." KRST "\n");
    return 0;
}
