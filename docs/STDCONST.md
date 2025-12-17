# Frigo's Standard Constants Library in C (stdconst)
 Uma biblioteca *header-only* em C que fornece constantes matemáticas fundamentais pré-calculadas com precisão máxima para formatos IEEE 754.

 Focada em otimização, esta biblioteca elimina a necessidade de cálculos de inicialização ou chamadas de função custosas (como `sqrt` ou `atan`) para valores constantes, garantindo que o compilador utilize o valor imediato exato.

 **Destaques:**

 * **Precisão Estendida:** Constantes geradas para preencher exatamente a mantissa de `float` (24 bits) e `double` (53 bits).
 * **Tipagem Explícita:** Sufixos claros (`_32` para `float` e `_64` para `double`) evitam conversões implícitas indesejadas.
 * **Otimização:** Inclui versões "Inversas" 1/x para substituir divisões lentas por multiplicações rápidas.
 * **Sem Dependências:** Basta copiar `stdconst.h` para o seu projeto.

---

## Constantes Circulares
 Definições de alta precisão para π e seus derivados. Essenciais para qualquer cálculo envolvendo trigonometria, física, ondas ou rotações 3D.

 * **PI:** A constante clássica.
 * **TAU:** Equivalente a 2π. Simplifica fórmulas que definem um ciclo completo (360°).
 * **INV_PI:** O inverso 1/π, útil para normalização de períodos.

### API
 ```c
 // Uso em rotações (float otimizado)
 float circunferencia = raio * TAU_32;

 // Normalização de ângulo (double preciso)
 double fator = angulo * INV_PI_64;
```

---

## Conversão de Ângulos
 Macros multiplicativas para converter rapidamente entre Graus e Radianos. Em computação gráfica e motores de jogos, esta é uma das operações mais frequentes.

 * **RAD2DEG:** Multiplique radianos por isso para obter graus.
 * **DEG2RAD:** Multiplique graus por isso para obter radianos.

### Exemplo
 ```c
 float angulo_graus = 90.0f;

 // Converte para radianos usando multiplicação (rápido)
 // Em vez de: (angulo_graus / 180.0f) * PI
 float angulo_rad = angulo_graus * DEG2RAD_32;
 ```

---

## Raízes Quadradas de Primos
 Raízes quadradas dos primeiros números primos, calculadas com precisão estendida. Estas constantes aparecem frequentemente em geometria vetorial e física.

 * **SQRT2 (√2):** Diagonal do quadrado unitário.
 * **SQRT3 (√3):** Diagonal do cubo unitário.
 * **SQRT5 (√5):** Base para o cálculo da Proporção Áurea.
 * **SQRT7 - SQRT13:** Úteis para sementes e constantes mágicas.

### API
 ```c
 // Calcula a diagonal de um quadrado de lado L
 float diagonal = lado * SQRT2_32;
 ```

---

## Inversos da Raiz Quadrada
 O inverso multiplicativo (1/√x) das constantes acima.

 **Por que usar?**

 Em processadores modernos, a **divisão** é uma instrução significativamente mais lenta que a **multiplicação**. Ao utilizar o inverso pré-calculado, você ganha performance em loops críticos.

 * **Otimização:** Substitui `x / sqrt(k)` por `x * INV_SQRT_K`.

### Exemplo de Otimização
 ```c
 // Lento (Divisão)
 vec.x = val / SQRT2_32;

 // Rápido (Multiplicação)
 vec.x = val * INV_SQRT2_32;
 ```

---

## Primos de Mersenne
 Números da forma 2ⁿ − 1. Em binário, eles consistem em uma sequência contínua de bits `1`.

 * **Uso:** Máscaras de bits (Bitmasks) e operações de módulo otimizado.
 * **Propriedade:** Calcular `x % MERSENNE_N` pode ser feito via bitwise `x & MERSENNE_N` se o divisor fosse uma potência de 2, mas aqui servem principalmente como máscaras de "todos os bits setados" até certa largura.

### API
 ```c
 // Seleciona apenas os 5 bits inferiores (0x1f)
 uint32_t low_bits = valor & MERSENNE_5;

 // Seleciona os 31 bits inferiores (limpa o bit de sinal em int32)
 uint32_t positivo = valor & MERSENNE_31;
 ```

---

## Proporção Áurea (Phi Inverso)
 Constantes baseadas na Proporção Áurea (φ ou ϕ), especificamente a parte fracionária escalonada para inteiros de 32 e 64 bits.

 Estas constantes são "maximamente irracionais", o que significa que elas dispersam bits melhor do que qualquer outro valor quando usadas em multiplicações cumulativas.

 * **Valor:** ((√5 - 1) / 2) × (2^Width)
 * **Uso:** Algoritmos de Hashing (Multiplicative Hashing), Fibonacci Hashing e dispersão de cores.

### API
 ```c
 // Dispersão de bits para Tabelas Hash
 uint32_t hash_index = (valor * PHI_INV_32) >> shift;
 ```
