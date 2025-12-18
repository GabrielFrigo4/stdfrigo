# Frigo's Standard Hash Library in C (stdhash)
 Parte da suíte **stdfrigo**. Uma biblioteca de alto desempenho para hashing não-criptográfico, implementando o estado da arte em algoritmos de software (**WyHash**) e aceleração nativa via hardware (**CRC32-C**).

 **Destaques:**

 * **Algoritmos Modernos:** Implementa **WyHash**, superando algoritmos antigos como FNV-1a e Jenkins em velocidade e qualidade estatística (passa no SMHasher).
 * **Polimorfismo (C11/C++):** Macros `_Generic` e sobrecarga de funções permitem uma API unificada para hashes de 32 e 64 bits.
 * **Hardware:** Suporte nativo a instruções **SSE4.2** para processamento de dados em escala de Gigabytes por segundo.
 * **NeoLibC Style:** Interface baseada em ponteiros de saída para consistência total com a suíte `stdrand`.

---

## Tipos de Hashing

### 1. Hashing de Inteiros (Mixers)
 Funções O(1) projetadas para transformar IDs numéricos ou ponteiros em distribuições uniformes, eliminando padrões sequenciais.

 * **32 bits:** Baseado no "lowbias32" (Chris Wellons).
 * **64 bits:** Baseado no "Stafford Mix 13" (usado no SplitMix64).

### 2. Fast Software Hash (`hash_fast`)
 Implementação do **WyHash**. É o algoritmo de software mais rápido da atualidade que mantém integridade estatística total.

 * **Algoritmo:** WyHash (Variante 128-bit mix).
 * **Uso:** Tabelas hash genéricas, strings e estruturas de dados complexas.

### 3. Hardware Hash (`hash_hw`)
 Utiliza instruções dedicadas da CPU para calcular o polinômio **CRC32-C (Castagnoli)**.

 * **Algoritmo:** CRC32-C (Aceleração SSE4.2).
 * **Vantagem:** Desempenho extremo ao processar grandes buffers diretamente no pipeline da CPU.

---

## API Unificada (Polimorfismo Total)
 A biblioteca utiliza detecção de tipos em tempo de compilação. O tipo da variável de saída determina automaticamente qual algoritmo será executado.

 ```c
 // 1. Hashing de Inteiros
 uint32_t h32 = hash_int((uint32_t)12345);
 uint64_t h64 = hash_int((uint64_t)12345);

 // 2. Hashing de Buffers (Software)
 // Exige 3 argumentos: buffer, tamanho e ponteiro de saída.
 uint64_t resultado;
 hash_fast(meu_buffer, len, &resultado);

 // 3. Combinação de Hashes (Composição de Chaves)
 // Útil para criar chaves compostas (ex: Coordenada X + Y)
 uint32_t seed = hash_int(x);
 uint32_t final = hash_combine(seed, hash_int(y));
 ```

---

## Hardware Hashing (x86-64)
 Acesso direto às instruções de hardware. A biblioteca inclui salvaguardas internas para detectar suporte a SSE4.2 em tempo de execução via `CPUID`.

### Uso Seguro
 Diferente das versões em software, as funções de hardware retornam um `bool` para indicar se a operação foi possível na CPU atual.

 ```c
 uint64_t out;
 if (hash_hw(meu_buffer, len, &out)) {
     // Sucesso: Processado via Hardware (SSE4.2)
 } else {
     // Fallback: CPU antiga, use a versão de software
     hash_fast(meu_buffer, len, &out);
 }
 ```

 * **Nota Técnica:** A versão de 64 bits utiliza múltiplos fluxos paralelos (streams) para mitigar a latência das instruções e maximizar a vazão de dados.

---

## Guia de Implementação
 A biblioteca é desenhada para ser eficiente tanto em C quanto em C++.

 * **Em C11:** Utiliza `_Generic` para selecionar a função correta.
 * **Em C++:** Utiliza sobrecarga de funções (*Overloading*) e templates com `if constexpr` para garantir overhead zero.

 ```c
 // Exemplo de robustez: aceita char, short, int, long long...
 // O polimorfismo resolve o tamanho correto do hash automaticamente.
 uint32_t h = hash_int((char)'A'); 
 ```
