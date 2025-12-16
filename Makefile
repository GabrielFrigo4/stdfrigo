PREFIX ?= /usr/local
INCLUDEDIR ?= $(PREFIX)/include

INSTALL ?= install
RM ?= rm -f

HEADER = stdrand.h

.PHONY: all install uninstall check

all:
	@echo "Esta é uma biblioteca header-only."
	@echo "Para instalar, execute: sudo make install"

install:
	@echo "Instalando $(HEADER) em $(INCLUDEDIR)..."
	@mkdir -p $(INCLUDEDIR)
	$(INSTALL) -m 644 $(HEADER) $(INCLUDEDIR)/$(HEADER)
	@echo "Instalação concluída."

uninstall:
	@echo "Removendo $(HEADER) de $(INCLUDEDIR)..."
	$(RM) $(INCLUDEDIR)/$(HEADER)
	@echo "Desinstalação concluída."

check:
	@echo "Verificando integridade básica..."
	@if [ -f $(HEADER) ]; then echo "OK: Arquivo $(HEADER) encontrado."; else echo "ERRO: $(HEADER) não encontrado."; exit 1; fi
