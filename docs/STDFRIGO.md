# Frigo's Standard Core Header (stdfrigo)

O componente central da suíte **stdfrigo**. Este cabeçalho atua como o ponto de entrada unificado (**Umbrella Header**) para toda a biblioteca, gerenciando inclusões automáticas, definições globais e controle de versão.

**Destaques:**

- **Single Include:** Acesso imediato a todos os módulos (`stdrand`, `stdhash`, `stdconst`) através de uma única diretiva `#include <stdfrigo.h>`.
- **Versionamento Semântico:** Macros pré-definidas para verificação de compatibilidade da API em tempo de compilação.
- **MSVC Auto-Link:** Detecção automática do compilador Microsoft Visual C++ para linkagem implícita da biblioteca estática via `#pragma comment`.

---

## Estrutura de Inclusão

Ao incluir `stdfrigo.h`, o pré-processador carrega automaticamente a pilha completa de funcionalidades na ordem de dependência correta.

| Módulo       | Descrição                                                          | Link                          |
| :----------- | :----------------------------------------------------------------- | :---------------------------- |
| **stdconst** | Constantes matemáticas IEEE 754 de precisão máxima.                | [📖 STDCONST.md](STDCONST.md) |
| **stdhash**  | Hashing polimórfico (WyHash) e aceleração de hardware (CRC32).     | [📖 STDHASH.md](STDHASH.md)   |
| **stdrand**  | Geradores aleatórios xoshiro/xoroshiro com estado de 128/256 bits. | [📖 STDRAND.md](STDRAND.md)   |

---

## Controle de Versão

O arquivo auxiliar `stdfrigo_defs.h` exporta definições que permitem à aplicação verificar a versão da biblioteca instalada. Isso é essencial para manter a estabilidade da API em projetos de longo prazo ou criar guardas de compatibilidade.

### Macros de Versão

| Macro                | Descrição                                      | Exemplo |
| :------------------- | :--------------------------------------------- | :------ |
| `STDFRIGO_VER_MAJOR` | Mudanças que quebram a API (Breaking Changes). | `1`     |
| `STDFRIGO_VER_MINOR` | Novas funcionalidades (Retrocompatível).       | `0`     |
| `STDFRIGO_VER_PATCH` | Correções de bugs e otimizações internas.      | `0`     |

### Macros de Formatação

| Macro                  | Tipo     | Descrição                                                      |
| :--------------------- | :------- | :------------------------------------------------------------- |
| `STDFRIGO_VERSION`     | `string` | Representação textual da versão ("1.0.0").                     |
| `STDFRIGO_VERSION_HEX` | `int`    | Representação hexadecimal para comparações rápidas (0x010000). |

**Exemplo de Verificação:**

```c
#include <stdfrigo.h>

#if STDFRIGO_VER_MAJOR >= 2
    // Código para a versão 2.0+
#else
    // Código legado para a versão 1.x
#endif

printf("Library Version: %s\n", STDFRIGO_VERSION);
```

---

## Suporte a Windows (MSVC)

Para desenvolvedores utilizando o compilador da Microsoft (`cl.exe`) ou Visual Studio, o cabeçalho inclui diretivas automáticas para facilitar a linkagem.

```c
#if defined(_MSC_VER)
#pragma comment(lib, "libstdfrigo.a")
#endif
```

Isso significa que, ao adicionar o diretório da biblioteca ao seu projeto, você não precisa adicionar `libstdfrigo.a` manualmente nas configurações do Linker; o cabeçalho instrui o compilador a buscar o arquivo `.a` automaticamente.

---

## Exemplo de Integração

O exemplo abaixo demonstra como o `stdfrigo.h` unifica a matemática, o hashing e a aleatoriedade em um fluxo de trabalho coeso com verificação de versão.

```c
/* * Inclusão Única:
 * Traz stdrand, stdhash, stdconst e definições de versão.
 */
#include <stdfrigo.h>
#include <stdio.h>

int main(void) {
    // 1. Verificar Versão
    printf("Iniciando stdfrigo v%s...\n", STDFRIGO_VERSION);

    // 2. STDRAND: Inicializa gerador com seed de hardware
    rand64_t rng = rand64_init(rand64_hw_seed());

    // 3. STDCONST: Usa constante TAU (2*PI) para cálculos
    double angulo = rand_double_range(&rng, 0.0, TAU_64);

    // 4. STDHASH: Gera um hash do resultado
    uint64_t hash = hash_int((uint64_t)angulo);

    printf("Angulo: %.5f rad | Hash: %llx\n", angulo, hash);

    return 0;
}
```
