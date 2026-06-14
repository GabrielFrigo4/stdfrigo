# BSDS (BSD Data Structures)

**O Manifesto e Guia Definitivo de Design**

A **LibBSDS** é uma biblioteca de estruturas de dados e algoritmos escrita em C. Ela não é apenas uma coleção de vetores e listas; é uma biblioteca construída sobre uma fundação filosófica rígida. O objetivo é ser o **padrão ouro** em previsibilidade, segurança, performance e modularidade no ecossistema C.

---

## 1. Por que "BSD"?

O nome "BSDS" não significa que a biblioteca roda exclusivamente em sistemas operacionais BSD (ela é totalmente compatível com Linux, Windows/MSYS2, macOS, etc.). O nome é uma homenagem e um compromisso com a **Filosofia BSD** de desenvolvimento de software.

A família de sistemas operacionais BSD (Berkeley Software Distribution) é historicamente conhecida por ter bases de código extremamente limpas, unificadas e pragmáticas. A LibBSDS herda os três pilares dos principais projetos BSD:

- **[OpenBSD](https://www.openbsd.org/): Segurança e Correção.** O código deve ser seguro por padrão, auditável e proativo na prevenção de falhas de memória (como _Integer Overflows_ e uso de memória não inicializada).
- **[FreeBSD](https://www.freebsd.org/): Performance e Modularidade.** O sistema não deve impor gargalos. Tudo deve ser modular, permitindo que o usuário injete alocadores de alta performance (como `jemalloc` ou _Arena Allocators_) conforme a necessidade.
- **[NetBSD](https://www.netbsd.org/): Portabilidade e Simplicidade.** O código deve rodar em qualquer lugar, escrito em C padrão (C99/C11), sem depender de magias obscuras específicas de um único compilador.

---

## 2. A Filosofia BSDS

Para manter o "Padrão Ouro", todo o código escrito para a LibBSDS deve seguir estritamente estes mandamentos:

### I. Nenhuma Alocação Oculta

Inspirado no design de linguagens modernas para sistemas (como Zig), a BSDS **nunca** chama `malloc` ou `free` globalmente pelas costas do usuário. Toda estrutura de dados exige um `bsds_allocator` explícito. Se o usuário quiser usar a alocação do sistema, ele deve passar o alocador padrão de forma consciente.

### II. Segurança Contra Overflows

Nunca confiamos em `size_t bytes = count * sizeof(type)`. Multiplicações podem sofrer _overflow_ silencioso em C, resultando em vulnerabilidades críticas. Toda alocação na BSDS exige separadamente a **quantidade** de elementos e o **tamanho** do elemento, simulando o comportamento da função `reallocarray` do OpenBSD.

### III. Destruição Segura de Dados (Zeroing)

Dados sensíveis não devem vazar. A biblioteca prevê o uso de limpezas seguras de memória antes da liberação, garantindo que o compilador não otimize (ignore) a limpeza, protegendo contra vazamentos em _core dumps_.

### IV. Espaço de Nomes Estrito (KISS)

O C não possui _namespaces_. Para evitar o "dependency hell" (conflitos de linker quando duas bibliotecas usam o mesmo nome de função), **absolutamente todas** as funções, _structs_ e _typedefs_ internos obrigatoriamente começam com o prefixo `bsds_`.

### V. Falhas Graciosas, Nunca Fatais

Se ocorrer falta de memória (OOM), a biblioteca **nunca** deve chamar `exit()`, `abort()` ou travar o programa. O erro deve ser propagado de volta (retornando `NULL` ou códigos de erro POSIX padrão, como `ENOMEM` ou `EINVAL`), delegando ao programador a decisão de como o software deve reagir.

---

## 3. O Coração da Biblioteca: A Interface do Allocator

Tudo na BSDS gira em torno da interface de alocação de memória. Isso garante isolamento de _threads_, facilidade para testes unitários (rastreio de _leaks_) e suporte a padrões de alta performance (como _Arenas_ ou _Pools_).

```c
#include <stddef.h>

// A Interface Central
typedef struct {
    void* (*alloc)(size_t count, size_t size, void* user_data);
    void* (*realloc)(void* ptr, size_t count, size_t size, void* user_data);
    void  (*free)(void* ptr, void* user_data);

    // Opcional: Garante que a memória seja sobrescrita com zeros antes
    // de ser liberada (estilo explicit_bzero). Vital para Hash Tables com senhas.
    void  (*free_secure)(void* ptr, size_t size, void* user_data);

    void* user_data; // Permite gerenciar o estado de allocators customizados
} bsds_allocator;

// O Fallback Seguro (Exposto pela biblioteca para facilidade de uso)
extern bsds_allocator BSDS_DEFAULT_ALLOCATOR;
```

_Nota de implementação:_ O `BSDS_DEFAULT_ALLOCATOR` internamente utiliza `calloc` (para garantir memória zerada) e possui proteções contra _Integer Overflow_ antes de repassar os dados para o `realloc` do sistema, caso o SO não possua `reallocarray` nativo.

---

## 4. O Sistema "Opt-In": Conforto vs. Segurança

A BSDS é rígida por padrão, exigindo chamadas verbosas como `bsds_vector_push()`. No entanto, ela oferece ao usuário a liberdade de usar o pré-processador do C para obter uma experiência de linguagem moderna, **desde que ele aceite os riscos de forma consciente (Opt-In)**.

A biblioteca expõe macros de configuração de ambiente:

### A. `BSDS_GENERICS` (Polimorfismo em C)

Utiliza o `_Generic` do C11 para rotear funções baseadas no tipo de dado, eliminando a necessidade de lembrar o nome exato da função para cada estrutura, **mas mantendo a proteção do prefixo**.

```c
#define BSDS_GENERICS
#include <bsds_vector.h>
#include <bsds_list.h>

// Uso:
bsds_push(meu_vetor, 10); // Roteado para bsds_vector_push
bsds_push(minha_lista, 20); // Roteado para bsds_list_push
```

### B. `BSDS_NO_PREFIX` (O Modo "YOLO")

Se o usuário estiver criando um script rápido ou tiver absoluta certeza de que não haverá conflitos no seu projeto, ele pode remover a barreira de segurança visual. O código fica extremamente limpo.

```c
#define BSDS_NO_PREFIX
#include <bsds_vector.h>

int main() {
    vector v; // 'bsds_' foi removido do tipo
    vector_init(&v, NULL);
    vector_push(&v, 42); // Chamada direta
    return 0;
}
```

### C. A Experiência Definitiva (Os Dois Juntos)

Ao combinar as duas _flags_, a LibBSDS se comporta como uma biblioteca nativa de linguagens modernas, garantindo código extremamente limpo, mas ainda respaldado por alocadores e segurança C11 por baixo dos panos.

```c
#define BSDS_NO_PREFIX
#define BSDS_GENERICS
#include <bsds_vector.h>
#include <bsds_list.h>

int main() {
    vector meu_vetor;
    vector_init(&meu_vetor, NULL); // Usa o alocador padrão internamente

    list minha_lista;
    list_init(&minha_lista, NULL);

    // O mesmo comando serve para qualquer estrutura!
    push(&meu_vetor, 42);
    push(&minha_lista, 99);

    return 0;
}
```

---

## 5. Diretrizes para Contribuidores / Criadores de Módulos

Ao adicionar uma nova estrutura de dados (ex: Hash Table, Árvore Red-Black) ao projeto BSDS, siga este checklist rigoroso:

1. **Arquivos:** Separe a interface em `include/bsds_nome.h` e a implementação em `src/bsds_nome.c`.
2. **Encapsulamento por Honra:** Como o C não tem modificadores de acesso (`private`), as variáveis internas das _structs_ (como `length`, `capacity`, `data`) não devem ser alteradas diretamente pelo usuário da biblioteca, devendo sempre ser acessadas via funções de leitura (ex: `bsds_vector_length()`).
3. **Estado Interno:** A _struct_ principal da estrutura deve **sempre** armazenar um ponteiro para o `bsds_allocator` usado na sua inicialização. Todas as futuras operações de inserção/deleção devem usar este ponteiro armazenado.
4. **Segurança de Inicialização:** Se o usuário passar `NULL` como alocador na função de inicialização (`init` ou `create`), atribua silenciosamente o ponteiro para `&BSDS_DEFAULT_ALLOCATOR`.
5. **Macros Cautelosas:** Nunca exponha macros que modifiquem variáveis locais invisíveis e não utilize `_Generic` fora das _flags_ Opt-In.
6. **Limpeza (`free`):** Garanta que a função que destrói a estrutura não deixe ponteiros pendentes para trás (defina-os como `NULL` após a liberação) para evitar ataques de _Use-After-Free_.

---

_Construído com respeito ao programador. Simples, explícito e indestrutível._
