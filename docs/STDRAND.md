# Frigo's Standard Random Library in C (stdrand)
 Parte da suíte **stdfrigo**. Uma biblioteca de alto desempenho para geração de números pseudoaleatórios (PRNG), implementando os algoritmos da família **xoshiro/xoroshiro**, atualmente considerados o estado da arte em velocidade e qualidade estatística.

 **Destaques:**

 * **Algoritmos Modernos:** Implementa **xoshiro** (XOR/Shift/Rotate), superiores em qualidade e performance aos antigos Mersenne Twister ou XORShift simples.
 * **Polimorfismo (C11):** Macros `_Generic` permitem usar uma API unificada (`rand_next`, `rand_jump`) para qualquer tipo de gerador.
 * **Zero-Safe:** Inicialização interna via `SplitMix64`, garantindo que o estado nunca seja inválido, mesmo com seeds simples.
 * **Hardware:** Suporte seguro e automático a `RDRAND`/`RDSEED` em CPUs x86-64.

---

## Geradores Disponíveis

### 1. Random 32 bits (`rand32_t`)
 Implementação do **xoshiro128****.

 É o gerador de 32 bits de propósito geral mais rápido e robusto disponível. Passa em todos os testes estatísticos (BigCrush).

 * **Algoritmo:** xoshiro128**
 * **Saída:** 32 bits (`uint32_t`)
 * **Estado:** 128 bits (4x `uint32_t`)

### 2. Random 64 bits (`rand64_t`)
 Implementação do **xoshiro256****.
 
 O "cavalo de batalha" da biblioteca. Recomendado para a grande maioria das aplicações, simulações e jogos. Possui um período massivo e passa em todos os testes estatísticos.

 * **Algoritmo:** xoshiro256**
 * **Saída:** 64 bits (`uint64_t`)
 * **Estado:** 256 bits (4x `uint64_t`)

### 3. Random Float (`rand_float_t`)
 Implementação do **xoshiro128+** (Plus).
 
 Variante otimizada para velocidade e geração de ponto flutuante. Retorna valores normalizados no intervalo **[0, 1)**.

 * **Algoritmo:** xoshiro128+
 * **Saída:** 32 bits (`float`)
 * **Estado:** 128 bits (4x `uint32_t`)

### 4. Random Double (`rand_double_t`)
 Implementação do **xoroshiro128+** (Plus).

 Variante leve de 64 bits otimizada para gerar números de dupla precisão normalizados no intervalo **[0, 1)**.

 * **Algoritmo:** xoroshiro128+
 * **Saída:** 64 bits (`double`)
 * **Estado:** 128 bits (2x `uint64_t`)

---

## API Unificada (C11 Generic)
 Se o seu compilador suportar C11, você pode usar as macros genéricas abaixo para manipular qualquer um dos geradores (`rand32`, `rand64`, `rand_float` ou `rand_double`) de forma transparente.

 ```c
 // 1. Inicialização (Construtor)
 rand64_t rng = rand64_init(12345ULL);
 rand_float_t rng_f = rand_float_init(12345ULL);

 // 2. Geração Básica
 uint64_t val = rand_next(&rng);
 float val_f = rand_next(&rng_f); // Retorna [0.0, 1.0)

 // 3. Reinicialização (Re-seed)
 // Reinicia o estado do gerador com uma nova semente.
 // Equivalente a chamar 'randX_init' novamente na mesma variável.
 rand_seed(&rng, nova_seed);

 // 4. Limites e Intervalos (Polimorfismo Total)
 // Inteiros
 uint64_t d100 = rand_bound(&rng, 100);
 uint64_t range = rand_range(&rng, 10, 20);

 // Floats (Funciona igual!)
 float f_bound = rand_bound(&rng_f, 100.0f);
 float f_range = rand_range(&rng_f, 10.5f, 20.0f);
 ```

### Funções de Salto (Jump Ahead)
 Útil para paralelismo. A função `rand_jump` avança o estado do gerador o equivalente a **2⁶⁴ chamadas** (ou 2⁹⁶ dependendo do algoritmo) em tempo constante. Isso permite garantir que threads diferentes operem em sub-sequências não sobrepostas.

 ```c
 // Na Thread A: usa rng original

 // Na Thread B:
 rand64_t rng_B = rng_A; // Copia estado
 rand_jump(&rng_B);      // Avança bilhões de passos
 // Agora rng_B pode ser usado sem colidir com rng_A
 ```

---

## Geração de Ponto Flutuante
 Diferente das bibliotecas padrão que exigem casts manuais e divisões lentas, a **stdrand** fornece tipos dedicados (`rand_float` e `rand_double`) que geram valores normalizados IEEE 754 diretamente.

 * **`rand_float_t`**: Preenche os 24 bits da mantissa de um `float`.
 * **`rand_double_t`**: Preenche os 53 bits da mantissa de um `double`.

 ```c
 rand_double_t rng = rand_double_init(seed);

 // Gera double de alta qualidade [0, 1) sem divisões
 double x = rand_next(&rng);
 ```

---

## Hardware Random (x86-64)
 Acesso às instruções de entropia da Intel/AMD (`RDRAND`/`RDSEED`). As funções possuem proteções internas (`CPUID`) para evitar *crashes* em CPUs antigas que não suportam estas instruções.

### 1. Pseudo-Aleatório Rápido (`_fast`)
 Usa `RDRAND` (AES-CTR em hardware). Muito rápido, ideal para preencher buffers ou seeds rápidas.

 ```c
 uint64_t val;
 if (rand64_hw_fast(&val)) { /* Sucesso */ }
 ```

### 2. Entropia Pura (`_entropy`)
 Usa `RDSEED` (Ruído térmico). Mais lento, mas criptograficamente seguro e não determinístico.

 ```c
 uint64_t seed;
 if (rand64_hw_entropy(&seed)) { /* Sucesso */ }
 ```

### 3. Auto Seed (`_seed`)
 A melhor maneira de inicializar seus geradores. Tenta obter entropia pura; se falhar, tenta pseudo-hardware; se falhar, usa o contador de ciclos da CPU (`RDTSC`) misturado com um hash robusto.

 ```c
 // Inicialização "Best Effort"
 rand64_t rng = rand64_init(rand64_hw_seed());
 ```
