PREFIX ?= /usr/local
INCLUDEDIR ?= $(PREFIX)/include

INSTALL ?= install
RM ?= rm -f

SRC_HEADERS = $(wildcard src/*.h)
INSTALLED_HEADERS = $(notdir $(SRC_HEADERS))

UNAME_S := $(shell uname -s)
ifneq (,$(findstring MINGW,$(UNAME_S))$(findstring MSYS,$(UNAME_S)))
	INSTALL_CMD = make install
else
	INSTALL_CMD = sudo make install
endif

.PHONY: all install uninstall check

all:
	@echo "Coleção de bibliotecas header-only: $(INSTALLED_HEADERS)"
	@echo "Origem: $(SRC_HEADERS)"
	@echo "Para instalar, execute: $(INSTALL_CMD)"

install:
	@echo "Instalando headers em $(INCLUDEDIR)..."
	@mkdir -p $(INCLUDEDIR)
	$(INSTALL) -m 644 $(SRC_HEADERS) $(INCLUDEDIR)
	@echo "Instalação concluída."

uninstall:
	@echo "Removendo headers de $(INCLUDEDIR)..."
	cd $(INCLUDEDIR) && $(RM) $(INSTALLED_HEADERS)
	@echo "Desinstalação concluída."

check:
	@echo "Verificando instalação..."
	@for h in $(INSTALLED_HEADERS); do \
		if [ -f $(INCLUDEDIR)/$$h ]; \
		then echo "OK: Arquivo $$h encontrado."; \
		else echo "ERRO: $$h não encontrado em $(INCLUDEDIR)."; exit 1; \
		fi \
	done
