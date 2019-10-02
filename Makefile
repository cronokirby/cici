CC=gcc
CCFLAGS= -Wall -Wpedantic -Wextra

debug: CCFLAGS += -g
debug: executable

prod: CCFLAGS += -O3
prod: executable

executable:
	$(CC) $(CCFLAGS) cici.c -o cici
