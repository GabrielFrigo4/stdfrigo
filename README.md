# Frigo's Standard Random Library in C (stdrand)
 Uma biblioteca *header-only* em C de alto desempenho para geração de números pseudoaleatórios (PRNG). Esta biblioteca implementa a família de algoritmos **XORShift**, conhecidos por sua velocidade extrema e implementação compacta.

 **Destaques:**
 * **Zero-Safe:** Utiliza internamente o algoritmo `SplitMix64` para inicialização (seeding), garantindo que o estado interno nunca fique zerado ou inválido, mesmo com seeds simples.
 * **Portátil:** Escrita em C99 padrão (`<stdint.h>`).
 * **Sem dependências:** Basta copiar `stdrand.h` para o seu projeto.

---

## Random 32 bits
 Implementação do algoritmo **XORShift32**. Ideal para sistemas embarcados com pouca memória ou processadores de 32 bits onde a velocidade é crítica e o período curto não é um problema.

 * **Algoritmo:** XORShift32 (Marsaglia)
 * **Período:** $2^{32} - 1$
 * **Estado:** 4 bytes (`uint32_t`)

### API
 ```c
 // Estrutura de estado
 rand32_t rand32;

 // Inicialização (Seed)
 // Usa SplitMix internamente para expandir a seed de forma robusta
 rand32_seed(&rand32, 2147483647);

 // Geração
 uint32_t num = rand32_next(&rand32);
 ```

-----

## Random 64 bits
 Implementação do algoritmo **XORShift64**. É o padrão equilibrado para uso geral. Oferece um período longo o suficiente para a maioria das aplicações (jogos, embaralhamento de listas) com performance máxima em CPUs modernas de 64 bits.

 * **Algoritmo:** XORShift64 (Marsaglia)
 * **Período:** $2^{64} - 1$
 * **Estado:** 8 bytes (`uint64_t`)

### API
 ```c
 // Estrutura de estado
 rand64_t rand64;

 // Inicialização (Seed)
 rand64_seed(&rand64, 2305843009213693951ULL);

 // Geração
 uint64_t num = rand64_next(&rand64);
 ```

-----

## Random 128 bits
 Implementação do algoritmo **XORShift128+ (Plus)**. Esta variante adiciona uma soma não-linear (`+`) na saída, melhorando drasticamente a qualidade estatística e passando em testes rigorosos como o BigCrush.

 Recomendado para simulações científicas ou quando se necessita de uma qualidade superior de aleatoriedade (especialmente para gerar `double` no intervalo [0, 1)).

 * **Algoritmo:** XORShift128+ (Vigna)
 * **Período:** $2^{128} - 1$
 * **Estado:** 16 bytes (`uint64_t[2]`)
 * **Saída:** Retorna 64 bits por chamada.

### API
 ```c
 // Estrutura de estado
 rand128_t rand128;

 // Inicialização (Seed)
 // O SplitMix inicializa ambas as partes do estado de 128 bits
 rand128_seed(&rand128, 2305843009213693951ULL);

 // Geração
 uint64_t num = rand128_next(&rand128);
 ```

-----

## Conversão para Ponto Flutuante
 Para utilizar os geradores em simulações que requerem números reais no intervalo `[0, 1)`, a biblioteca fornece macros otimizadas que mapeiam os bits gerados diretamente para a mantissa do formato IEEE 754.

### Como funciona
 O método realiza um *bit shift* para descartar bits de baixa entropia ou que excedam a precisão do formato, e então multiplica por um fator de normalização ($2^{-53}$ para double ou $2^{-24}$ para float).

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
