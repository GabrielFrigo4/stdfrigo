# Frigo's Standard Hash Library in C (stdhash)
 Uma biblioteca *header-only* em C de alto desempenho para algoritmos de **Hashing Não-Criptográfico**. Esta biblioteca implementa algoritmos padrão da indústria focados em velocidade extrema e distribuição uniforme (efeito avalanche) para uso em tabelas hash, filtros de Bloom e verificações de integridade.

 **Destaques:**

 * **Alta Performance:** Algoritmos otimizados para operações bitwise rápidas, evitando overhead de funções complexas.
 * **Portátil:** Escrita em C99 padrão (`<stdint.h>`, `<stddef.h>`).
 * **Sem Dependências:** Basta copiar `stdhash.h` para o seu projeto.

---

## Hashing de Inteiros (Mixers)
 Funções O(1) sem loops. Projetadas para transformar um número inteiro (que pode ter padrões sequenciais ou bits zerados) em um índice aleatório e uniformemente distribuído.

 Baseadas nos **Finalizers do MurmurHash3**, estas funções são ideais para usar IDs numéricos ou ponteiros como chaves em tabelas hash.

### Mixers 32 bits
 ```c
 // De 32 para 32 bits (MurmurHash3 Finalizer)
 // Transforma sequências (1, 2, 3...) em números pseudoaleatórios distantes
 uint32_t h1 = hash32_u32(12345U);

 // De 64 para 32 bits (Fold + Mix)
 // Útil para hashear ponteiros em sistemas 64-bit para tabelas de 32-bit
 uint64_t ptr_val = (uint64_t)ptr;
 uint32_t h2 = hash32_u64(ptr_val);

```

### Mixers 64 bits
```c
// De 64 para 64 bits (MurmurHash3 Finalizer)
// Transforma sequências (1, 2, 3...) em números pseudoaleatórios distantes
uint64_t h3 = hash64_u64(999999ULL);

```

---

## Hashing de Dados (Buffers)
 Funções projetadas para processar sequências de bytes de tamanho arbitrário (strings, arquivos, estruturas).

### Jenkins One-at-a-Time
 Um algoritmo robusto que garante um forte efeito avalanche. Cada bit da entrada afeta aproximadamente 50% dos bits de saída.

 * **Algoritmo:** Jenkins One-at-a-Time (Adaptado)
 * **Uso:** Tabelas hash genéricas, dados binários variados.
 * **Funções:** `hash32_buff` e `hash64_buff`

 ```c
 struct Pacote pkt = { ... };
 // Gera um hash de 32 bits
 uint32_t hash32 = hash32_buff(&pkt, sizeof(pkt));
 // Gera um hash de 64 bits
 uint64_t hash64 = hash64_buff(&pkt, sizeof(pkt));
 ```

### FNV-1a (Fast Hash)
 Implementação do algoritmo **Fowler–Noll–Vo**. É extremamente simples e eficiente, ideal para strings curtas e caminhos de arquivos.

 * **Algoritmo:** FNV-1a
 * **Uso:** Chaves de texto (strings), URLs.
 * **Funções:** `fasthash32_buff` e `fasthash64_buff`

 ```c
 const char *key = "usuario_123";
 // Gera um hash de 32 bits
 uint32_t hash = fasthash32_buff(key, strlen(key));
 // Gera um hash de 64 bits
 uint64_t hash = fasthash64_buff(key, strlen(key));
 ```

---

## Combinação de Hashes
 Funções auxiliares para combinar dois valores de hash em um único valor. Essencial para criar chaves compostas (ex: Coordenada X + Coordenada Y).

 A implementação utiliza um "Magic Number" baseado na **Razão Áurea** (φ⁻¹ ou ϕ⁻¹) para evitar simetrias e colisões quando os valores de entrada são permutados ou similares.

### API
 ```c
 // Exemplo: Hash de uma coordenada 2D (x, y)

 // 1. Combine 32 bits
 uint32_t seed32 = hash32_u32(x);
 uint32_t final32 = hash32_combine(seed32, hash32_u32(y));

 // 2. Combine 64 bits
 uint64_t seed64 = hash64_u64(x);
 uint64_t final64 = hash64_combine(seed64, hash64_u64(y));
 ```

---

## Hardware Hashing (x86-64)
 Para processadores modernos (Intel Nehalem+ ou AMD Bulldozer+), a biblioteca oferece acesso direto às instruções de aceleração de hardware **SSE4.2**.

 **Nota de Portabilidade:** Estas funções são protegidas por guardas de pré-processador (`#if defined(__x86_64__)`). Em arquiteturas não suportadas, elas simplesmente não são compiladas.

### Destaques
 * **Zero Config:** Utiliza `__attribute__((target("sse4.2")))` (em GCC/Clang) para habilitar as instruções apenas no escopo destas funções.
 * **Velocidade Extrema:** Utiliza o hardware dedicado para calcular CRC32, sendo até 10x mais rápido que algoritmos de software.

### Algoritmo: CRC32-C (Castagnoli)
 Implementa o polinômio Castagnoli (usado em iSCSI e redes modernas), que possui melhores propriedades de detecção de erros que o CRC32 padrão (IEEE).

 * **`hash32_hw`**: Cálculo direto do CRC32-C.
 * **`hash64_hw`**: Implementação paralela que mantém dois fluxos de CRC (um normal e um invertido) para gerar um hash robusto de 64 bits usando primitivas de 32 bits.

 ```c
 #if defined(__x86_64__)
    char big_buffer[4096];

    // Hash ultra-rápido para grandes volumes de dados
    uint32_t checksum = hash32_hw(big_buffer, sizeof(big_buffer));

    // Versão de 64 bits (combina dois streams de CRC)
    uint64_t signature = hash64_hw(big_buffer, sizeof(big_buffer));
 #endif
 ```
