DIR := ${CURDIR}
DEPS_S = $(wildcard Server/*.cpp Server/*.h)
DEPS_C = $(wildcard Client/*.cpp Client/*.h)

all: client server

client: $(DEPS_C)
	$(MAKE) -C $(DIR)/Client

server: $(DEPS_S)
	$(MAKE) -C $(DIR)/Server

.PHONY: clean
clean:
	$(MAKE) -C ${DIR}/Client clean
	$(MAKE) -C ${DIR}/Server clean
	@echo -e "\n\033[1;32m[Cleaning]\033[0m"
	rm -f server client