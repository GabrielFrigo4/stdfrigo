
# Frigo's Standard Constants Library in C (stdconst)
 Parte da suíte **stdfrigo**. Uma biblioteca que fornece constantes matemáticas fundamentais pré-calculadas com precisão máxima para formatos IEEE 754, eliminando erros de arredondamento e custos de inicialização.

 **Destaques:**

 * **Precisão Máxima:** Constantes geradas para preencher exatamente a mantissa de `float` (24 bits) e `double` (53 bits).
 * **Tipagem Explícita:** Sufixos claros (`_32` para `float` e `_64` para `double`) que evitam conversões implícitas e garantem o uso de literais imediatos.
 * **Otimização por Inversão:** Fornece versões `X_INV` (1/x) para substituir divisões lentas por multiplicações rápidas em loops críticos.
 * **Filosofia NeoLibC:** Sem dependências externas, garantindo que o compilador insira os valores diretamente no binário.

---

## Categorias de Constantes

### 1. Constantes Circulares (Trigonometria)
 Definições de alta precisão para π, τ (2\π) e suas conversões. Essenciais para física, ondas e rotações 3D.

 * **Exemplo:** `PI_64`, `TAU_32`, `DEG2RAD_64`.
 * **Otimização:** Use `RAD2DEG` para converter radianos em graus sem realizar a divisão manualmente.

### 2. Raízes Quadradas e Inversos
 Valores de raízes comuns e seus inversos multiplicativos.

 * **Raízes:** `SQRT2` (√2), `SQRT3`, `SQRT5`.
 * **Inversos:** `SQRT2_INV` (1/√2), fundamental para normalização de vetores unitários.

### 3. Proporção Áurea (Phi Inverso)
 Constantes baseadas na Proporção Áurea (ϕ⁻¹), amplamente utilizadas em algoritmos de espalhamento (Hashing) e Fibonacci.

 * **Uso:** A constante `PHI_INV_HASH_64` é a base para os mixers de alta performance da `stdhash`.

### 4. Primos de Mersenne
 Números da forma 2ⁿ - 1. Em binário, consistem em uma sequência contínua de bits `1`.

 * **Uso:** Máscaras de bits (Bitmasks) e operações de módulo otimizado.
 * **Exemplo:** `MERSENNE_31` (limpa o bit de sinal em `int32_t`).

---

## Exemplo de Aplicação

### Otimização de Performance
 Substituir uma divisão por uma multiplicação é uma das otimizações mais simples e eficazes em arquiteturas modernas.

 ```c
 #include "stdconst.h"

 // Lento: Realiza uma divisão em tempo de execução
 float força = total / SQRT2_32;

 // Rápido: O compilador usa um valor imediato e realiza uma multiplicação
 float força_opt = total * INV_SQRT2_32;
 ```

### Máscaras de Bits com Mersenne
 Ideal para garantir que valores permaneçam dentro de intervalos de potência de 2.

 ```c
 // Garante que o índice esteja entre 0 e 31 (5 bits)
 uint32_t index = hash & MERSENNE_5;
 ```

---

## Convenção de Nomes
 Para evitar qualquer ambiguidade, a biblioteca segue um padrão rígido de sufixos:

 | Sufixo | Tipo C | Precisão |
 | --- | --- | --- |
 | `_32` | `float` | 24-bit mantissa (7-8 dígitos decimais) |
 | `_64` | `double` | 53-bit mantissa (15-17 dígitos decimais) |

---

## Design e Portabilidade
 A `stdconst` é puramente baseada em definições de pré-processador.

 * **Zero Overhead:** Não ocupa espaço na memória global (seção de dados); os valores são injetados diretamente no código máquina.
 * **Compatibilidade:** Funciona em qualquer compilador C ou C++ que suporte literais de ponto flutuante.
 * **Arquitetura:** Pronta para sistemas embarcados e CPUs de alta performance.
