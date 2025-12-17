# Frigo's Standard Random Library in C (stdrand)
 Uma biblioteca *header-only* em C de alto desempenho para geração de números pseudoaleatórios (PRNG). Esta biblioteca implementa a família de algoritmos **XORShift**, conhecidos por sua velocidade extrema e implementação compacta.

 **Destaques:**

 * **API Moderna (Construtores):** Inicialização via retorno de estrutura (`_init`), evitando erros com variáveis não inicializadas.
 * **Zero-Safe:** Utiliza internamente o algoritmo `SplitMix64` para inicialização, garantindo que o estado interno nunca fique zerado ou inválido, mesmo com seeds simples.
 * **Portátil:** Escrita em C99 padrão (`<stdint.h>`, `<stdbool.h>`).
 * **Sem Dependências:** Basta copiar `stdrand.h` para o seu projeto.

---

## Random 32 bits
 Implementação do algoritmo **XORShift32**. Ideal para sistemas embarcados com pouca memória ou processadores de 32 bits onde a velocidade é crítica e o período curto não é um problema.

 * **Algoritmo:** XORShift32 (Marsaglia)
 * **Período:** 2³² − 1
 * **Estado:** 4 bytes (`uint32_t`)

### API
 ```c
 // 1. Inicialização (Style: Construtor)
 // Retorna uma struct pronta para uso.
 rand32_t rng = rand32_init(12345U);

 // 2. Geração
 uint32_t num = rand32_next(&rng);

```

---

## Random 64 bits
 Implementação do algoritmo **XORShift64**. É o padrão equilibrado para uso geral. Oferece um período longo o suficiente para a maioria das aplicações (jogos, embaralhamento de listas) com performance máxima em CPUs modernas de 64 bits.

 * **Algoritmo:** XORShift64 (Marsaglia)
 * **Período:** 2⁶⁴ − 1
 * **Estado:** 8 bytes (`uint64_t`)

### API
 ```c
 // 1. Inicialização
 rand64_t rng = rand64_init(99999ULL);

 // 2. Geração
 uint64_t num = rand64_next(&rng);
 ```

---

## Random 128 bits
 Implementação do algoritmo **XORShift128+ (Plus)**. Esta variante adiciona uma soma não-linear (`+`) na saída, melhorando drasticamente a qualidade estatística e passando em testes rigorosos como o BigCrush.

 Recomendado para simulações científicas ou quando se necessita de uma qualidade superior de aleatoriedade (especialmente para gerar `double` no intervalo [0, 1)).

 * **Algoritmo:** XORShift128+ (Vigna)
 * **Período:** 2¹²⁸ - 1
 * **Estado:** 16 bytes (`uint64_t[2]`)
 * **Saída:** Retorna 64 bits por chamada.

### API
 ```c
 // 1. Inicialização
 // O SplitMix expande a seed única para preencher os 128 bits de estado
 rand128_t rng = rand128_init(123456789ULL);

 // 2. Geração
 uint64_t num = rand128_next(&rng);
 ```

---

## Conversão para Ponto Flutuante
 Para utilizar os geradores em simulações que requerem números reais no intervalo `[0, 1)`, a biblioteca fornece macros otimizadas que mapeiam os bits gerados diretamente para a mantissa do formato IEEE 754.

### Como funciona
 O método realiza um *bit shift* para descartar bits de baixa entropia ou que excedam a precisão do formato, e então multiplica por um fator de normalização (2^{-53} para double ou 2^{-24} para float).

### Exemplos
 **1. Gerando `double` [0, 1)**

 Ideal para alta precisão. Utiliza o gerador de 64 bits (ou 128+).
 ```c
 // Usa os 53 bits mais significativos de um uint64_t
 // Equivale a: (x >> 11) * 0x1.0p-53
 double val_f64 = TO_DBL_01(rand128_next(&rand128));
 ```

 **2. Gerando `float` [0, 1)**

 Ideal para aplicações gráficas (OpenGL/DirectX) ou economia de memória. Pode usar o gerador de 32 bits.
 ```c
 // Usa os 24 bits mais significativos de um uint32_t
 // Equivale a: (x >> 8) * 0x1.0p-24f
 float val_f32 = TO_FLT_01(rand32_next(&rand32));
 ```

---

## Hardware Random (x86-64)
 Para processadores modernos (Intel Ivy Bridge+ ou AMD Zen+), a biblioteca oferece acesso direto aos geradores de hardware (DRNG).

 **Nota de Portabilidade:** Estas funções são protegidas por guardas de pré-processador (`#if defined(__x86_64__)`). Em arquiteturas não suportadas (como ARM ou 32-bit), elas simplesmente não são compiladas. Recomendamos verificar a arquitetura antes de chamá-las ou usar macros de detecção.

### Destaques
 * **Zero Config:** Utiliza `__attribute__((target))` (em GCC/Clang) para habilitar as instruções `RDRAND/RDSEED` apenas no escopo destas funções.
 * **Hierarquia Clara:** Funções divididas por propósito (`_fast` para velocidade, `_entropy` para qualidade, `_seed` para conveniência).

---

### 1. Pseudo-Aleatório Rápido (RDRAND)
 Acessa o **CSPRNG** embutido no hardware.

 * **Sufixo:** `_fast`
 * **Características:** Extremamente rápido, estatisticamente sólido.
 * **Retorno:** `true` (sucesso) ou `false` (falha de hardware).

 ```c
 uint64_t val;
 if (rand64_hw_fast(&val)) {
     // Uso direto do valor
 }
 ```

### 2. Entropia Pura (RDSEED)
 Obtém entropia diretamente do ruído térmico do circuito.

 * **Sufixo:** `_entropy`
 * **Características:** Mais lento, sujeito a esgotamento (underflow).
 * **Uso:** Sementes criptográficas.

 ```c
 uint64_t seed;
 if (rand64_hw_entropy(&seed)) {
     // Temos uma semente verdadeira
 }
 ```

### 3. Auto Seed (Wrapper Inteligente)
 A função recomendada para inicializar os geradores de software. Ela implementa uma estratégia de "Melhor Esforço" (Best Effort) para garantir que você sempre tenha uma semente válida.

 **Estratégia de Fallback:**

 1. Tenta Entropia Pura (`RDSEED`).
 2. Se falhar, tenta Pseudo-Hardware (`RDRAND`).
 3. Se falhar, usa Ciclos da CPU (`RDTSC`) misturados via SplitMix.

 ```c
 // Inicializa o gerador de 64 bits com a melhor seed disponível no sistema
 rand64_t rng = rand64_init(rand64_hw_seed());

 // Inicializa o gerador de 128 bits
 rand128_t rng128 = rand128_init(rand64_hw_seed());
 ```
