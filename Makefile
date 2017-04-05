DIR := ${CURDIR}

all: client server

client:
	$(MAKE) -C $(DIR)/Client

server:
	$(MAKE) -C $(DIR)/Server

.PHONY: clean
clean:
	$(MAKE) -C ${DIR}/Client clean
	$(MAKE) -C ${DIR}/Server clean
	@echo -e "\n\033[1;32m[Cleaning]\033[0m"
	rm -f server client