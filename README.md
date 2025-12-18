# Frigo's Standard Library in C (stdfrigo)
 **stdfrigo** é uma coleção de bibliotecas em C (C99) focadas em **alta performance**, **simplicidade** e **zero dependências**. Projetada sob a filosofia **NeoLibC**, a suíte oferece algoritmos fundamentais com interfaces polimórficas (C11/C++) para máxima ergonomia e desempenho.

 O objetivo deste projeto é fornecer implementações de algoritmos fundamentais (geração de números aleatórios, hashing, matemática) que sejam fáceis de integrar em qualquer projeto, sem a complexidade de sistemas de build pesados ou bibliotecas externas.

---

## 📚 Módulos Disponíveis
 Atualmente, a biblioteca é composta pelos seguintes módulos:

### 1. `stdrand.h` (Random)
 Geradores de números pseudoaleatórios (PRNG) baseados na família **xoshiro/xoroshiro**, o estado da arte em qualidade estatística e velocidade.
 * **Algoritmos:** xoshiro128**, xoshiro256**, xoshiro128+ e xoroshiro128+.
 * **Hardware:** Suporte seguro a `RDRAND` e `RDSEED` com proteção via `CPUID`.
 * **API:** Inicialização via `SplitMix64`, funções de salto (*jump*) para paralelismo e suporte a limites (*bounds*) sem viés.
 * [📖 STDRAND.md](docs/STDRAND.md)

### 2. `stdhash.h` (Hashing)
 Algoritmos de hash não-criptográficos otimizados para velocidade e dispersão uniforme (Efeito Avalanche).
 * **Algoritmos:** **WyHash** (Software) para hashing de buffers e mixers estatísticos de O(1) para inteiros.
 * **Hardware:** Aceleração via instruções **SSE4.2** (CRC32-C Castagnoli) com processamento em múltiplos fluxos paralelos.
 * **Funcionalidades:** API polimórfica que seleciona o algoritmo com base no tipo da variável de saída (32 ou 64 bits).
 * [📖 STDHASH.md](docs/STDHASH.md)

### 3. `stdconst.h` (Constants)
 Constantes matemáticas fundamentais pré-calculadas com precisão máxima para o padrão IEEE 754.
 * **Matemática:** PI, Tau, Raízes Quadradas e Proporção Áurea (ϕ⁻¹).
 * **Otimização:** Versões inversas (1/x) para substituir divisões lentas por multiplicações rápidas.
 * **Bitmasks:** Primos de Mersenne para máscaras de bits eficientes.
 * [📖 STDCONST.md](docs/STDCONST.md)

---

## 🚀 Instalação e Integração
 A suíte foi desenhada para suportar compilação em unidades de tradução separadas conforme o projeto cresce.

 Para instalar globalmente (padrão `/usr/local/include`), utilize o `Makefile`:

 O sistema de instalação detecta automaticamente o ambiente (Linux/macOS ou MSYS2/Windows) para ajustar as permissões.
 ```bash
 # Instalar
 make install

 # Verificar se tudo foi instalado corretamente
 make check

 # Desinstalar
 make uninstall
```

---

## 🔧 Configuração de Ambiente (Windows / MSYS2)
 Se você estiver utilizando **MSYS2** ou compilando no **Windows** e o compilador não encontrar os arquivos `.h` (erro `No such file or directory`), pode ser necessário adicionar o caminho de instalação (`/usr/local/include`) às variáveis de ambiente.

### 1. No Terminal MSYS2 (Bash)
 Para que o `gcc` ou `clang` encontre as bibliotecas automaticamente, adicione as variáveis ao seu arquivo de configuração do shell (ex: `.bashrc` ou `.zshrc`):
 ```bash
 export C_INCLUDE_PATH="$(cygpath -m /usr/local/include)"
 export CPLUS_INCLUDE_PATH="$(cygpath -m /usr/local/include)"
 export LIBRARY_PATH="$(cygpath -m /usr/local/lib)"
 export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig"
 fcc() {
     gcc "$@" $(pkg-config --cflags --libs stdfrigo)
 }
 ```

 *Após adicionar, reinicie o terminal ou rode `source ~/.bashrc` ou  `source ~/.zshrc`.*

### 2. No Windows (Registro)
 Se preferir configurar globalmente para o Windows (para usar via CMD ou PowerShell fora do MSYS2), você pode criar um arquivo `.reg`.

 > **Nota:** Verifique se o caminho do seu MSYS2 é realmente `C:\msys64`. Se for diferente, ajuste os caminhos abaixo.

 Crie um arquivo chamado `config_env.reg` com o seguinte conteúdo e execute-o:
 ```reg
 Windows Registry Editor Version 5.00

 [HKEY_CURRENT_USER\Environment]
 "C_INCLUDE_PATH"="C:\\msys64\\usr\\local\\include"
 "CPLUS_INCLUDE_PATH"="C:\\msys64\\usr\\local\\include"
 "LIBRARY_PATH"="C:\\msys64\\usr\\local\\lib"
 "PKG_CONFIG_PATH"="C:\\msys64\\usr\\local\\lib\\pkgconfig"
 ```

---

## ⚡ Exemplo de Uso
 A API unificada permite integrar hardware, software e matemática de forma transparente:

 ```c
 #include <stdio.h>
 #include <stdrand.h>
 #include <stdhash.h>
 #include <stdconst.h>

 int main() {
     // 1. Inicialização com hardware entropy (Best Effort)
     rand64_t rng = rand64_init(rand64_hw_seed()); //

     // 2. Geração e Hashing Polimórfico
     uint64_t random_val = rand_next(&rng); //
     uint64_t hash_res;
     hash_fast(&random_val, sizeof(random_val), &hash_res); //

     // 3. Otimização com Constantes
     // Multiplicação por inverso é mais rápido que divisão por raiz
     double res = (double)hash_res * INV_SQRT2_64; 

     printf("Random: %llu | Hash: %llu | Calc: %.5f\n", random_val, hash_res, res);
     return 0;
 }
 ```

## ⚖️ Licença
 Este projeto é de domínio público ou licenciado sob MIT. Sinta-se livre para integrar em projetos comerciais ou privados.
