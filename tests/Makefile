TARGETS = legacy_server
#TARGETS = legacy_client
#TARGETS = vtl_quic_server

OBJS = ${TARGETS:=.o}

LIBS_DIR = ../lib

CC := gcc

CFLAGS += -g -Wall -Wextra

LDFLAGS ?= -L$(LIBS_DIR)

LIBS := -l:libquicly.a -l:libpicotls-core.a -l:libpicotls-openssl.a
LIBS += -lcrypto


all: $(TARGETS)
$(TARGETS): $(OBJS)
					$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)
