# Frigo's Standard Library in C (stdfrigo)
 **stdfrigo** é uma coleção de bibliotecas em C (C99) focadas em **alta performance**, **simplicidade** e **zero dependências**. Projetada sob a filosofia **NeoLibC**, a suíte oferece algoritmos fundamentais com interfaces polimórficas (C11/C++) para máxima ergonomia e desempenho.

 O objetivo deste projeto é fornecer implementações de algoritmos fundamentais (geração de números aleatórios, hashing, matemática) que sejam fáceis de integrar em qualquer projeto, sem a complexidade de sistemas de build pesados ou bibliotecas externas.

---

## 📚 Módulos Disponíveis
 Atualmente, a biblioteca é composta pelos seguintes módulos:

### 0. `stdfrigo.h` (Core & Umbrella)
 O cabeçalho central da suíte. Atua como um **ponto único de inclusão** ("Umbrella Header") para facilitar o uso da biblioteca completa e gerenciar definições compartilhadas entre os módulos.

 * **Inclusão Unificada:** Inclui automaticamente `stdrand.h`, `stdhash.h` e `stdconst.h`, permitindo acesso a toda a API com um único `#include`.
 * **Definições Base:** Centraliza macros de detecção de plataforma (Linux/Windows), atributos de compilador e suporte a linkagem automática no MSVC.
 * **Versionamento:** Define a versão semântica da biblioteca e flags globais de configuração para controle de compatibilidade.
 * [📖 STDFRIGO.md](docs/STDFRIGO.md)

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

 Para instalar globalmente (padrão `/usr/local/bin`, `/usr/local/lib` e `/usr/local/include`), utilize o `Makefile`. O sistema detecta automaticamente o ambiente (Linux/MacOS ou MSYS2/Windows) para ajustar as permissões.

 ```bash
 # Instalar
 make install

 # Verificar se tudo foi instalado corretamente
 make check

 # Desinstalar
 make uninstall
 ```

---

## 🛠️ Como Compilar
 Após a instalação, a **stdfrigo** oferece quatro formas flexíveis de integração, desde wrappers automáticos até flags manuais:

### 1. Wrappers Automáticos (Recomendado)
 A instalação disponibiliza comandos que configuram automaticamente as flags de inclusão e linkagem. Eles repassam todos os argumentos para o compilador nativo (`gcc` ou `g++`).

 * **`fcc`**: Wrapper para projetos em C (invoca `gcc`).
 * **`f++`** ou **`fpp`**: Wrapper para projetos em C++ (invoca `g++`).

 ```bash
 fcc main.c -o app
 f++ main.cpp -o app
 fpp main.cpp -o app
 ```

### 2. Via `pkg-config` (Portável)
 Ideal para **Makefiles**, scripts de automação ou sistemas de build. Este método garante a portabilidade entre diferentes sistemas operacionais.

 ```bash
 gcc main.c -o app $(pkg-config --cflags --libs stdfrigo)
 ```

### 3. Flag Curta (`-lf`)
 Para agilidade no terminal, a instalação cria um alias de linkagem (`libf.a`). É a forma mais rápida de linkar manualmente.

 ```bash
 gcc main.c -o app -lf
 ```

### 4. Flag Padrão (`-lstdfrigo`)
 O método tradicional e explícito, utilizando o nome completo da biblioteca.

 ```bash
 gcc main.c -o app -lstdfrigo
 ```

---

## 🔧 Configuração de Ambiente (Windows / MSYS2)
 Se você estiver utilizando **MSYS2** ou compilando no **Windows**, é possível que o sistema não encontre os comandos instalados (`fcc`, `f++`) ou os arquivos de cabeçalho (erro `No such file or directory`). Isso ocorre porque o caminho `/usr/local` nem sempre está no *path* padrão dessas ferramentas.

 Para corrigir, adicione os caminhos de **Binários, Includes e Libs** às suas variáveis de ambiente:

### 1. No Terminal MSYS2 (Bash)
 Para que o shell encontre os comandos `fcc/f++` e o compilador encontre as bibliotecas automaticamente, adicione as linhas abaixo ao seu arquivo de configuração (ex: `.bashrc` ou `.zshrc`):

 ```bash
 # 1. Executáveis (fcc, f++, fpp)
 export PATH="$PATH:$(cygpath -m /usr/local/bin)"

 # 2. Headers C e C++ (.h)
 export C_INCLUDE_PATH="$(cygpath -m /usr/local/include)"
 export CPLUS_INCLUDE_PATH="$(cygpath -m /usr/local/include)"

 # 3. Bibliotecas Estáticas (.a)
 export LIBRARY_PATH="$(cygpath -m /usr/local/lib)"

 # 4. Integração com Pkg-Config
 export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig"
 ```

 *Após adicionar, reinicie o terminal ou rode `source ~/.bashrc`.*

### 2. No Windows (PowerShell / CMD)
 Se você deseja usar o **fcc** ou **f++** diretamente pelo PowerShell ou CMD (fora do terminal MSYS2), você precisa adicionar o caminho do MSYS2 ao `Path` do Windows.

 > **Nota:** Verifique se o caminho do seu MSYS2 é realmente `C:\msys64`. Se for diferente, ajuste os caminhos abaixo.

 1. Abra o Menu Iniciar e digite **"Editar as variáveis de ambiente do sistema"** ou use o **PowerToys** da Microsoft.
 2. Clique em **Variáveis de Ambiente**.
 3. Em **Variáveis do Usuário** (parte de cima), selecione `Path` e clique em **Editar**.
 4. Clique em **Novo** e adicione:
 * `C:\msys64\usr\local\bin` (Para achar o `fcc.exe`, `f++.exe` e `fpp.exe`)
 5. Se for usar compiladores nativos fora do MSYS, adicione também novas variáveis para
 * `C_INCLUDE_PATH` apontando para `C:\msys64\usr\local\include`
 * `CPLUS_INCLUDE_PATH` apontando para `C:\msys64\usr\local\include`
 * `LIBRARY_PATH` apontando para `C:\msys64\usr\local\lib`.
 * `PKG_CONFIG_PATH` apontando para `C:\msys64\usr\local\pkgconfig`.

 Se preferir configurar globalmente para o Windows usando **Registros**, você pode criar um arquivo `.reg`.

 Crie um arquivo chamado `config.reg` com o seguinte conteúdo e execute-o:
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

---

## ⚖️ Licença
 Este projeto é de domínio público ou licenciado sob MIT. Sinta-se livre para integrar em projetos comerciais ou privados.
