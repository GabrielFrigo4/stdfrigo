PREFIX     ?= /usr/local
LIBDIR     ?= $(PREFIX)/lib
INCLUDEDIR ?= $(PREFIX)/include
BINDIR     ?= $(PREFIX)/bin

CC      ?= gcc
AR      ?= ar
RANLIB  ?= ranlib
INSTALL ?= install
RM      ?= rm -f

CFLAGS  += -O3 -Wall -Wextra -std=c23 -flto -Iinclude
HWFLAGS  = -msse4.2 -mrdrnd -mrdseed

ALIAS   = $(wildcard alias/*.c)
SRC     = $(wildcard src/*.c)
OBJ     = $(SRC:.c=.o)
LIBSTD  = libstdfrigo.a
LIBF    = libf.a
HEADERS = $(wildcard include/*.h)
PC_FILE = stdfrigo.pc

UNAME_S := $(shell uname -s)
ifneq (,$(findstring MINGW,$(UNAME_S))$(findstring MSYS,$(UNAME_S)))
    EXE_EXT     := .exe
    INSTALL_CMD := make install
else
    EXE_EXT     :=
    INSTALL_CMD := sudo make install
endif

.PHONY: all clean install uninstall check

all: $(PC_FILE) $(LIBSTD) fcc f++
	@echo "Biblioteca $(LIBSTD) e utilitários fcc/f++ gerados com sucesso."
	@echo "Para instalar, execute: $(INSTALL_CMD)"

$(PC_FILE):
	@echo "prefix=$(PREFIX)" > $@
	@echo "exec_prefix=\$${prefix}" >> $@
	@echo "libdir=\$${prefix}/lib" >> $@
	@echo "includedir=\$${prefix}/include" >> $@
	@echo "" >> $@
	@echo "Name: stdfrigo" >> $@
	@echo "Description: High performance NeoLibC suite" >> $@
	@echo "Version: 1.0.0" >> $@
	@echo "Cflags: -I\$${includedir} $(HWFLAGS)" >> $@
	@echo "Libs: -L\$${libdir} -lstdfrigo" >> $@

$(LIBSTD): $(OBJ)
	$(AR) rcs $@ $(OBJ)
	$(RANLIB) $@

fcc: alias/fcc.c
	$(CC) $(CFLAGS) $< -o $@

f++: alias/f++.c
	$(CC) $(CFLAGS) $< -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) $(HWFLAGS) -c $< -o $@

clean:
	$(RM) src/*.o $(LIBSTD) $(PC_FILE) fcc f++

install: all $(PC_FILE)
	@echo "Instalando em $(PREFIX)..."
	@mkdir -p $(INCLUDEDIR)
	@mkdir -p $(LIBDIR)/pkgconfig
	@mkdir -p $(BINDIR)
	$(INSTALL) -m 644 $(HEADERS) $(INCLUDEDIR)
	$(INSTALL) -m 644 $(LIBSTD) $(LIBDIR)
	ln -sf $(LIBDIR)/$(LIBSTD) $(LIBDIR)/$(LIBF)
	$(INSTALL) -m 644 $(PC_FILE) $(LIBDIR)/pkgconfig/
	$(INSTALL) -m 755 fcc$(EXE_EXT) $(BINDIR)/fcc$(EXE_EXT)
	$(INSTALL) -m 755 f++$(EXE_EXT) $(BINDIR)/f++$(EXE_EXT)
	ln -sf $(BINDIR)/f++ $(BINDIR)/fpp
	@echo "Instalação concluída."

uninstall: 
	@echo "Removendo arquivos de $(PREFIX)..."
	$(RM) $(addprefix $(INCLUDEDIR)/, $(notdir $(HEADERS))) 
	$(RM) $(LIBDIR)/$(LIBSTD) 
	$(RM) $(LIBDIR)/$(LIBF)
	$(RM) $(LIBDIR)/pkgconfig/$(PC_FILE)
	$(RM) $(BINDIR)/fcc $(BINDIR)/f++ $(BINDIR)/fpp 
	@echo "Desinstalação concluída."

check:
	@echo "Verificando instalação..."
	@for h in $(notdir $(HEADERS)); do \
		if [ -f $(INCLUDEDIR)/$$h ]; then echo "OK: Header $$h encontrado."; \
		else echo "ERRO: $$h ausente."; exit 1; fi; \
	done
	@if [ -f $(LIBDIR)/$(LIBSTD) ]; then echo "OK: Biblioteca $(LIBSTD) encontrada."; \
	else echo "ERRO: $(LIBSTD) ausente."; exit 1; fi
	@if [ -f $(LIBDIR)/pkgconfig/$(PC_FILE) ]; then echo "OK: Config $(PC_FILE) encontrada."; \
	else echo "ERRO: $(PC_FILE) ausente."; exit 1; fi
	@if [ -x $(BINDIR)/fcc ]; then echo "OK: Binário fcc encontrado."; \
	else echo "ERRO: fcc ausente."; exit 1; fi
	@if [ -x $(BINDIR)/f++ ]; then echo "OK: Binário f++ encontrado."; \
	else echo "ERRO: f++ ausente."; exit 1; fi
