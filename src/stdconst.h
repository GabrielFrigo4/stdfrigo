#ifndef STDCONST_H
#define STDCONST_H

#ifdef __cplusplus
extern "C"
{
#endif

    /* ===============================================================
     * CONSTANTES CIRCULARES (PI & TAU)
     * Uso: Trigonometria, Rotações, Física.
     * =============================================================== */

#define PI_32 3.14159265F
#define PI_64 3.14159265358979323846

#define TAU_32 6.28318530F
#define TAU_64 6.28318530717958647692

#define INV_PI_32 0.31830988F
#define INV_PI_64 0.31830988618379067153

    /* ===============================================================
     * CONVERSÃO DE ÂNGULOS
     * RAD2DEG: Multiplique radianos por isso para obter graus.
     * DEG2RAD: Multiplique graus por isso para obter radianos.
     * =============================================================== */

#define RAD2DEG_32 57.29577951F
#define RAD2DEG_64 57.2957795130823208768

#define DEG2RAD_32 0.01745329F
#define DEG2RAD_64 0.01745329251994329576

    /* ===============================================================
     * RAÍZES QUADRADAS (PRECISÃO ESTENDIDA)
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
     * INVERSO DA RAIZ QUADRADA (1 / sqrt(x))
     * Uso: Otimização. Multiplicar pelo inverso é mais rápido que dividir.
     * Exemplo: vec * INV_SQRT2 em vez de vec / SQRT2.
     * =============================================================== */

#define INV_SQRT2_32 0.70710678F
#define INV_SQRT2_64 0.7071067811865475244

#define INV_SQRT3_32 0.57735027F
#define INV_SQRT3_64 0.5773502691896257645

#define INV_SQRT5_32 0.44721360F
#define INV_SQRT5_64 0.4472135954999579393

#define INV_SQRT7_32 0.37796447F
#define INV_SQRT7_64 0.3779644730092272272

#define INV_SQRT11_32 0.30151134F
#define INV_SQRT11_64 0.3015113445777636226

#define INV_SQRT13_32 0.27735010F
#define INV_SQRT13_64 0.2773500981126145610

    /* ===============================================================
     * NÚMEROS PRIMOS DE MERSENNE (2^n - 1)
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
     * Uso: Hashing (dispersão de bits), Fibonacci, Geometria.
     * Valor: (sqrt(5) - 1) / 2 * 2^Width
     * =============================================================== */

#define PHI_INV_32 0x9e3779b9U
#define PHI_INV_64 0x9e3779b97f4a7c15ULL

#ifdef __cplusplus
}
#endif

#endif
