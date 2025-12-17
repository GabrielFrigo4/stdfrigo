# Frigo's Standard Library in C (stdfrigo)
 **stdfrigo** é uma coleção de bibliotecas *header-only* em C (C99) focadas em **alta performance**, **simplicidade** e **zero dependências**.

 O objetivo deste projeto é fornecer implementações de algoritmos fundamentais (geração de números aleatórios, hashing, matemática) que sejam fáceis de integrar em qualquer projeto, sem a complexidade de sistemas de build pesados ou bibliotecas externas.

---

## 📚 Módulos Disponíveis
 Atualmente, a biblioteca é composta pelos seguintes módulos:

### 1. `stdrand.h` (Random)
 Geradores de números pseudoaleatórios (PRNG) baseados na família **XORShift** e acesso a geradores de hardware (RDRAND/RDSEED).
 * **Algoritmos:** XORShift32, XORShift64, XORShift128+.
 * **Hardware:** Suporte nativo a instruções Intel/AMD (`_rdrand`, `_rdseed`) para entropia pura.
 * **API:** Inicialização segura (Zero-Safe) e construtores ergonômicos.
 * [📖 Ler Documentação Completa (STDRAND.md)](docs/STDRAND.md)

### 2. `stdhash.h` (Hashing)
 Algoritmos de hash não-criptográficos otimizados para velocidade e distribuição uniforme (Avalanche Effect).
 * **Algoritmos:** MurmurHash3 Finalizer, Jenkins One-at-a-Time, FNV-1a.
 * **Hardware:** Aceleração via instruções **SSE4.2** (CRC32-C) para hashear grandes buffers instantaneamente.
 * **Funcionalidades:** Hashing de inteiros O(1), buffers e combinação de hashes.
 * [📖 Ler Documentação Completa (STDHASH.md)](docs/STDHASH.md)

### 3. `stdconst.h` (Constants)
 Constantes matemáticas fundamentais pré-calculadas com precisão máxima para IEEE 754.
 * **Matemática:** PI, Tau, Raízes Quadradas (Primos), Proporção Áurea (Phi).
 * **Otimização:** Versões "Inversas" ($1/x$) para substituir divisões lentas por multiplicações.
 * **Bitmasks:** Primos de Mersenne para máscaras de bits rápidas.
 * [📖 Ler Documentação Completa (STDCONST.md)](docs/STDCONST.md)

---

## 🚀 Instalação
 Como as bibliotecas são *header-only*, você pode simplesmente copiar os arquivos `.h` para o seu projeto. No entanto, para instalar no sistema (padrão `/usr/local/include`), utilize o `Makefile` incluído.

 O sistema de instalação detecta automaticamente o ambiente (Linux/macOS ou MSYS2/Windows) para ajustar as permissões.
 ```bash
 # Instalar (copia todos os .h para o diretório de include do sistema)
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
 ```

---

## ⚡ Exemplo de Uso
 Após a instalação, basta incluir os headers desejados diretamente no seu código.

 Este exemplo demonstra a integração dos três módulos: geração de sementes via hardware, hashing de IDs e uso de constantes matemáticas para otimização.
 ```c
 #include <stdio.h>
 #include <stdrand.h>
 #include <stdhash.h>
 #include <stdconst.h>

 int main() {
     // 1. Inicializando o PRNG com entropia de hardware (Best Effort)
     // Usa RDSEED se disponível, caindo para RDTSC se necessário.
     rand64_t rng = rand64_init(rand64_hw_seed());

     // 2. Gerando um ID aleatório
     uint64_t random_id = rand64_next(&rng);

     // 3. Calculando o hash desse ID (Transformando em índice de tabela)
     uint64_t index = hash64_u64(random_id);

     // 4. Exemplo de Otimização Matemática
     // Em vez de: double val = (double)random_id / SQRT2_64;
     // Usamos multiplicação pelo inverso pré-calculado (mais rápido):
     double val = (double)random_id * INV_SQRT2_64;

     // 5. Output
     printf("ID Gerado : %llu\n", random_id);
     printf("Hash Index: %llu\n", index);
     printf("Calculo   : %.5f\n", val);
     printf("Mersenne13: %u (Mask: 0x%X)\n", MERSENNE_13, MERSENNE_13);

     return 0;
 }
 ```

## ⚖️ Licença
 Este projeto é de domínio público ou licenciado sob MIT (sinta-se livre para usar como quiser).
