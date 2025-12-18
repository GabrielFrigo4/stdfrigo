PREFIX     ?= /usr/local
LIBDIR     ?= $(PREFIX)/lib
INCLUDEDIR ?= $(PREFIX)/include

CC      ?= gcc
AR      ?= ar
RANLIB  ?= ranlib
INSTALL ?= install
RM      ?= rm -f

CFLAGS  += -O3 -Wall -Wextra -std=c11 -flto -Iinclude
HWFLAGS  = -msse4.2 -mrdrnd -mrdseed

SRC     = $(wildcard src/*.c)
OBJ     = $(SRC:.c=.o)
LIB     = libstdfrigo.a
HEADERS = $(wildcard include/*.h)
PC_FILE = stdfrigo.pc
ALIAS   = f

UNAME_S := $(shell uname -s)
ifneq (,$(findstring MINGW,$(UNAME_S))$(findstring MSYS,$(UNAME_S)))
    INSTALL_CMD = make install
else
    INSTALL_CMD = sudo make install
endif

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

.PHONY: all clean install uninstall check

all: $(LIB)
	@echo "Biblioteca $(LIB) gerada com sucesso."
	@echo "Para instalar, execute: $(INSTALL_CMD)"

$(LIB): $(OBJ)
	$(AR) rcs $@ $(OBJ)
	$(RANLIB) $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) $(HWFLAGS) -c $< -o $@

clean:
	$(RM) src/*.o $(LIB) $(PC_FILE)

install: all $(PC_FILE)
	@echo "Instalando em $(PREFIX)..."
	@mkdir -p $(INCLUDEDIR)
	@mkdir -p $(LIBDIR)/pkgconfig
	$(INSTALL) -m 644 $(HEADERS) $(INCLUDEDIR)
	$(INSTALL) -m 644 $(LIB) $(LIBDIR)
	ln -sf $(LIBDIR)/$(LIB) $(LIBDIR)/lib$(ALIAS).a
	$(INSTALL) -m 644 $(PC_FILE) $(LIBDIR)/pkgconfig/
	@echo "Instalação concluída."

uninstall: 
	@echo "Removendo arquivos de $(PREFIX)..."
	$(RM) $(addprefix $(INCLUDEDIR)/, $(notdir $(HEADERS))) 
	$(RM) $(LIBDIR)/$(LIB) 
	$(RM) $(LIBDIR)/lib$(ALIAS).a
	$(RM) $(LIBDIR)/pkgconfig/$(PC_FILE)
	@echo "Desinstalação concluída." 

check:
	@echo "Verificando instalação..."
	@for h in $(notdir $(HEADERS)); do \
		if [ -f $(INCLUDEDIR)/$$h ]; then echo "OK: Header $$h encontrado."; \
		else echo "ERRO: $$h ausente."; exit 1; fi; \
	done 
	@if [ -f $(LIBDIR)/$(LIB) ]; then echo "OK: Biblioteca $(LIB) encontrada."; \
	else echo "ERRO: $(LIB) ausente."; exit 1; fi
	@if [ -f $(LIBDIR)/pkgconfig/$(PC_FILE) ]; then echo "OK: Config $(PC_FILE) encontrada."; \
	else echo "ERRO: $(PC_FILE) ausente."; exit 1; fi
	@if [ -L $(LIBDIR)/libf.a ] || [ -f $(LIBDIR)/libf.a ]; then echo "OK: Atalho -lf encontrado."; \
	else echo "ERRO: Atalho -lf ausente."; exit 1; fi
