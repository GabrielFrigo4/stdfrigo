#ifndef STDCONST_H
#define STDCONST_H

#include <stdfrigo_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===============================================================
 * CONSTANTES CIRCULARES (PI & TAU)
 * ===============================================================
 * Uso: Trigonometria, Rotações, Física.
 * =============================================================== */

#define PI_32 3.14159265F
#define PI_64 3.14159265358979323846

#define TAU_32 6.28318530F
#define TAU_64 6.28318530717958647692

/* ===============================================================
 * INVERSO DAS CONSTANTES CIRCULARES (1/PI & 1/TAU)
 * ===============================================================
 * Uso: Trigonometria, Rotações, Física.
 * =============================================================== */

#define PI_INV_32 0.31830988F
#define PI_INV_64 0.31830988618379067153

#define TAU_INV_32 0.15915494F
#define TAU_INV_64 0.15915494309189533576

/* ===============================================================
 * CONVERSÃO DE ÂNGULOS (RADIANOS <-> GRAUS)
 * ===============================================================
 * RAD2DEG: Multiplique radianos por isso para obter graus.
 * DEG2RAD: Multiplique graus por isso para obter radianos.
 * =============================================================== */

#define RAD2DEG_32 57.29577951F
#define RAD2DEG_64 57.2957795130823208768

#define DEG2RAD_32 0.01745329F
#define DEG2RAD_64 0.01745329251994329576

/* ===============================================================
 * RAÍZES QUADRADAS (sqrt(x))
 * ===============================================================
 * Uso: Geometria, Física, Cálculos de Distância.
 * Sufixo F: Garante que sejam tratados como float (32-bit).
 * =============================================================== */

#define SQRT2_32 1.41421356F
#define SQRT2_64 1.4142135623730950488

#define SQRT3_32 1.73205081F
#define SQRT3_64 1.7320508075688772935

#define SQRT5_32 2.23606797F
#define SQRT5_64 2.2360679774997896964

#define SQRT7_32 2.64575131F
#define SQRT7_64 2.6457513110645905905

#define SQRT11_32 3.31662479F
#define SQRT11_64 3.3166247903554006152

#define SQRT13_32 3.60555127F
#define SQRT13_64 3.6055512754639892931

/* ===============================================================
 * INVERSO DAS RAÍZES QUADRADAS (1/sqrt(x))
 * ===============================================================
 * Uso: Otimização. Multiplicar pelo inverso é mais rápido que dividir.
 * Exemplo: vec * INV_SQRT2 em vez de vec / SQRT2.
 * =============================================================== */

#define SQRT2_INV_32 0.70710678F
#define SQRT2_INV_64 0.7071067811865475244

#define SQRT3_INV_32 0.57735027F
#define SQRT3_INV_64 0.5773502691896257645

#define SQRT5_INV_32 0.44721360F
#define SQRT5_INV_64 0.4472135954999579393

#define SQRT7_INV_32 0.37796447F
#define SQRT7_INV_64 0.3779644730092272272

#define SQRT11_INV_32 0.30151134F
#define SQRT11_INV_64 0.3015113445777636226

#define SQRT13_INV_32 0.27735010F
#define SQRT13_INV_64 0.2773500981126145610

/* ===============================================================
 * NÚMEROS PRIMOS DE MERSENNE (2^n - 1)
 * ===============================================================
 * Uso: Máscaras de bits (Bitmasks) e Otimizações de Módulo.
 * =============================================================== */

#define MERSENNE_2 0x3U
#define MERSENNE_3 0x7U
#define MERSENNE_5 0x1fU
#define MERSENNE_7 0x7fU
#define MERSENNE_13 0x1fffU
#define MERSENNE_17 0x1ffffU
#define MERSENNE_19 0x7ffffU
#define MERSENNE_31 0x7fffffffU
#define MERSENNE_61 0x1fffffffffffffffULL

/* ===============================================================
 * CONSTANTES MATEMÁTICAS (GOLDEN RATIO / PHI)
 * ===============================================================
 * 1. PHI (Math):     (1 + sqrt(5)) / 2     ~ 1.618
 * Uso: Geometria, Fibonacci, Física.
 *
 * 2. PHI_INV (Math): (sqrt(5) - 1) / 2     ~ 0.618
 * Uso: Otimização de divisões e proporções inversas.
 *
 * 3. HASH (Int/Hex): PHI_INV * 2^Width
 * Uso: Dispersão de bits (Multiplicative Hashing).
 * =============================================================== */

#define PHI_32 1.61803399F
#define PHI_64 1.61803398874989484820

#define PHI_INV_32 0.61803399F
#define PHI_INV_64 0.61803398874989484820

#define PHI_INV_HASH_32 0x9e3779b9U
#define PHI_INV_HASH_64 0x9e3779b97f4a7c15ULL

#ifdef __cplusplus
}
#endif

#endif
