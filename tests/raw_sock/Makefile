
OBJ := raw_sock.o

LIB_DIR = ../../include

CC := gcc
CFLAGS := -g -Wall

all: $(OBJ)

.PHONY: clean

clean:
	rm -f $(OBJ)


$(OBJ): raw_sock.c raw_sock.h $(LIB_DIR)/vtl.h
	$(CC) $(CFLAGS) -o $@ -c $<