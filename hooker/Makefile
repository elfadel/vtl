
ROOT_DIR = ..
LIBS_DIR = $(ROOT_DIR)/lib/
LIBBPF_DIR = $(ROOT_DIR)/lib/libbpf/src

CC := gcc

CFLAGS += -g -Wall -Wextra -Wpedantic \
          -Wformat=2 -Wno-unused-parameter -Wshadow \
          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
          -Wredundant-decls -Wnested-externs -Wmissing-include-dirs
CFLAGS += -Wnull-dereference -Wjump-misses-init -Wlogical-op
CFLAGS += -O2
CFLAGS += -I$(LIBBPF_DIR)/build/usr/include/ -g
CFLAGS += -I$(ROOT_DIR)/include/headers/
CFLAGS += -I$(ROOT_DIR)/deps

LDFLAGS ?= -L$(LIBS_DIR)

LIBS := -l:libbpf.a -l:libquicly.a -l:libpicotls-core.a -l:libpicotls-openssl.a
LIBS += -lelf
LIBS += -lz
LIBS += -lpthread
LIBS += -lcrypto
LIBS += -lsctp

all: hk-daemon
	@echo "Hooker daemon created."

.PHONY: clean

clean:
	rm -f *.o hk-daemon

hk-daemon: hooker.o 
	$(CC) $(LDFLAGS) -o $@ $< $(LIBS)

hooker.o: hooker.c
	$(CC) $(CFLAGS) -o $@ -c $<
