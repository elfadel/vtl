ROOT_DIR = ..

LIBBPF_DIR = $(ROOT_DIR)/lib/libbpf/src

CC := gcc
CFLAGS += -g -Wall -Wextra -Wpedantic \
          -Wformat=2 -Wno-unused-parameter -Wshadow \
          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
          -Wredundant-decls -Wnested-externs -Wmissing-include-dirs
CFLAGS += -Wnull-dereference -Wjump-misses-init -Wlogical-op
CFLAGS += -O2

# Because of automatic dependency generation
CFLAGS += -I$(LIBBPF_DIR)/build/usr/include/ -g
CFLAGS += -I$(ROOT_DIR)/include/headers/
CFLAGS += -I$(ROOT_DIR)/deps

DEPFLAGS = -MT $@ -MD -MP -MF $(DEP_DIR)/$*.Td


all: $(OBJS)

.PHONY: clean
clean:
	$(RM) -f $(OBJS)
	$(RM) -r $(DEP_DIR)

$(OBJS):%.o: %.c $(DEP_DIR)/%.d Makefile
	$(CC) $(DEPFLAGS) $(CFLAGS) -c -o $@ $<
	mv -f $(DEP_DIR)/$*.Td $(DEP_DIR)/$*.d


.PRECIOUS: $(DEP_DIR)/%.d
$(DEP_DIR)/%.d: ;
-include $(DEPS)
