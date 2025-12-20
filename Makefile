# ==============================================================================
# STDFRIGO MAKEFILE
# ==============================================================================

# 1. DIRETÓRIOS
PREFIX      ?= /usr/local
LIBDIR      ?= $(PREFIX)/lib
INCLUDEDIR  ?= $(PREFIX)/include
BINDIR      ?= $(PREFIX)/bin

# 2. FERRAMENTAS
CC      ?= gcc
AR      ?= ar
RANLIB  ?= ranlib
INSTALL ?= install
RM      ?= rm -f

# 3. FLAGS
CFLAGS  += -O3 -Wall -Wextra -std=c23 -flto -Iinclude

# 4. ARQUIVOS
SRC     = $(wildcard src/*.c)
OBJ     = $(SRC:.c=.o)
LIBSTD  = libstdfrigo.a
LIBF    = libf.a
HEADERS = $(wildcard include/*.h)
PC_FILE = stdfrigo.pc

# 5. DETECÇÃO DE OS
UNAME_S := $(shell uname -s)
ifneq (,$(findstring MINGW,$(UNAME_S))$(findstring MSYS,$(UNAME_S)))
    EXE_EXT     := .exe
    LINK_CMD    := cp -f
    CHECK_LINK  := test -f
else
    EXE_EXT     :=
    LINK_CMD    := ln -sf
    CHECK_LINK  := test -L
endif

# ==============================================================================
# REGRAS PRINCIPAIS
# ==============================================================================

.PHONY: all clean install uninstall check test

all: $(LIBSTD) $(PC_FILE) fcc f++
	@echo "=================================================="
	@echo " Build Completo: $(LIBSTD)"
	@echo " Utilitários:    fcc, f++"
	@echo "=================================================="

$(LIBSTD): $(OBJ)
	$(AR) rcs $@ $(OBJ)
	$(RANLIB) $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

fcc: alias/fcc.c
	$(CC) $(CFLAGS) $< -o $@

f++: alias/f++.c
	$(CC) $(CFLAGS) $< -o $@

$(PC_FILE):
	@echo "Generating $@..."
	@echo "prefix=$(PREFIX)" > $@
	@echo "exec_prefix=\$${prefix}" >> $@
	@echo "libdir=\$${prefix}/lib" >> $@
	@echo "includedir=\$${prefix}/include" >> $@
	@echo "" >> $@
	@echo "Name: stdfrigo" >> $@
	@echo "Description: High performance NeoLibC suite" >> $@
	@echo "Version: 1.0.0" >> $@
	@echo "Cflags: -I\$${includedir}" >> $@
	@echo "Libs: -L\$${libdir} -lstdfrigo" >> $@

# ==============================================================================
# INSTALAÇÃO
# ==============================================================================

install: all
	@echo "Instalando em $(DESTDIR)$(PREFIX)..."
	
	# 1. Criar Diretórios
	@mkdir -p $(DESTDIR)$(INCLUDEDIR)
	@mkdir -p $(DESTDIR)$(LIBDIR)/pkgconfig
	@mkdir -p $(DESTDIR)$(BINDIR)

	# 2. Instalar Headers
	$(INSTALL) -m 644 $(HEADERS) $(DESTDIR)$(INCLUDEDIR)

	# 3. Instalar Biblioteca e criar Link Simbólico (libf.a -> libstdfrigo.a)
	$(INSTALL) -m 644 $(LIBSTD) $(DESTDIR)$(LIBDIR)
	@cd $(DESTDIR)$(LIBDIR) && $(LINK_CMD) $(LIBSTD) $(LIBF)

	# 4. Instalar pkg-config
	$(INSTALL) -m 644 $(PC_FILE) $(DESTDIR)$(LIBDIR)/pkgconfig/

	# 5. Instalar Binários
	$(INSTALL) -m 755 fcc$(EXE_EXT) $(DESTDIR)$(BINDIR)/fcc$(EXE_EXT)
	$(INSTALL) -m 755 f++$(EXE_EXT) $(DESTDIR)$(BINDIR)/f++$(EXE_EXT)
	@cd $(DESTDIR)$(BINDIR) && $(LINK_CMD) f++$(EXE_EXT) fpp$(EXE_EXT)

	@echo "=========================================="
	@echo " INSTALL SUCCESS: Library ready."
	@echo " Location: $(DESTDIR)$(PREFIX)"
	@echo "=========================================="

uninstall:
	@echo "Removendo arquivos de $(DESTDIR)$(PREFIX)..."
	$(RM) $(addprefix $(DESTDIR)$(INCLUDEDIR)/, $(notdir $(HEADERS)))
	$(RM) $(DESTDIR)$(LIBDIR)/$(LIBSTD)
	$(RM) $(DESTDIR)$(LIBDIR)/$(LIBF)
	$(RM) $(DESTDIR)$(LIBDIR)/pkgconfig/$(PC_FILE)
	$(RM) $(DESTDIR)$(BINDIR)/fcc$(EXE_EXT)
	$(RM) $(DESTDIR)$(BINDIR)/f++$(EXE_EXT)
	$(RM) $(DESTDIR)$(BINDIR)/fpp$(EXE_EXT)

	@echo "=========================================="
	@echo " UNINSTALL COMPLETE: System cleaned."
	@echo " Todos os arquivos da lib foram removidos."
	@echo "=========================================="

clean:
	$(RM) src/*.o $(LIBSTD) $(LIBF) $(PC_FILE) fcc$(EXE_EXT) f++$(EXE_EXT) test1$(EXE_EXT)

	@echo "================================================="
	@echo " [CLEAN] Objetos, Libs e Executáveis removidos."
	@echo " Diretório limpo e pronto para recompilar."
	@echo "================================================="

# ==============================================================================
# TESTES
# ==============================================================================

check:
	@echo "Verificando integridade da instalação em $(DESTDIR)$(PREFIX)..."

	# 1. Verificar Headers (Loop por todos os arquivos definidos em HEADERS)
	@for h in $(notdir $(HEADERS)); do \
		if [ ! -f $(DESTDIR)$(INCLUDEDIR)/$$h ]; then \
			echo "ERRO CRÍTICO: Header '$$h' não foi encontrado em $(DESTDIR)$(INCLUDEDIR)."; \
			exit 1; \
		fi; \
	done
	@echo " [OK] Todos os headers encontrados."

	# 2. Verificar Bibliotecas (Arquivo real + Symlink)
	@test -f $(DESTDIR)$(LIBDIR)/$(LIBSTD) || (echo "ERRO: $(LIBSTD) ausente." && exit 1)
	@$(CHECK_LINK) $(DESTDIR)$(LIBDIR)/$(LIBF)   || (echo "ERRO: Symlink $(LIBF) quebrado ou ausente." && exit 1)
	@echo " [OK] Bibliotecas estáticas encontradas."

	# 3. Verificar Pkg-Config
	@test -f $(DESTDIR)$(LIBDIR)/pkgconfig/$(PC_FILE) || (echo "ERRO: Arquivo $(PC_FILE) ausente." && exit 1)
	@echo " [OK] Integração pkg-config confirmada."

	# 4. Verificar Binários e Symlinks
	@test -x $(DESTDIR)$(BINDIR)/fcc$(EXE_EXT) || (echo "ERRO: Binário 'fcc' ausente ou sem permissão de execução." && exit 1)
	@test -x $(DESTDIR)$(BINDIR)/f++$(EXE_EXT) || (echo "ERRO: Binário 'f++' ausente ou sem permissão de execução." && exit 1)
	@$(CHECK_LINK) $(DESTDIR)$(BINDIR)/fpp$(EXE_EXT) || (echo "ERRO: Symlink 'fpp' ausente." && exit 1)
	@echo " [OK] Utilitários de linha de comando encontrados."

	@echo "========================================"
	@echo " SUCESSO: Instalação 100% verificada."
	@echo "========================================"

test: $(LIBSTD)
	@echo "Compilando testes..."
	$(CC) $(CFLAGS) test/test1.c ./$(LIBSTD) -o test1
	@echo "Rodando testes..."
	./test1
