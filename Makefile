ROOT_DIR = .

BIN_DIR = $(ROOT_DIR)/bin
SRC_DIR = .
LIBTOOLS = $(ROOT_DIR)/lib
BPFPROG_DIR = $(ROOT_DIR)/XTFsPool
LIBBPF_DIR = $(ROOT_DIR)/lib/libbpf/src

MODULES := $(SRC_DIR)/service_api
MODULES += $(SRC_DIR)/cbr
MODULES += $(SRC_DIR)/dbr
MODULES += $(SRC_DIR)/net_monitor
MODULES += $(SRC_DIR)/launcher
MODULES += $(SRC_DIR)/ui
MODULES += $(SRC_DIR)/include/common

BIN_CLEAN = $(addsuffix _clean, $(BIN_DIR))
MODULES_CLEAN = $(addsuffix _clean, $(MODULES))
LIBTOOLS_CLEAN = $(addsuffix _clean, $(LIBTOOLS))
BPFPROG_CLEAN = $(addsuffix _clean, $(BPFPROG_DIR))

STATIC_LIBVTL = $(BIN_DIR)/libvtl.a

LIBVTL_OBJS := $(SRC_DIR)/service_api/*.o
LIBVTL_OBJS += $(SRC_DIR)/cbr/*.o
LIBVTL_OBJS += $(SRC_DIR)/dbr/*.o
LIBVTL_OBJS += $(SRC_DIR)/net_monitor/*.o
LIBVTL_OBJS += $(SRC_DIR)/launcher/*.o
LIBVTL_OBJS += $(SRC_DIR)/include/common/*.o

VTL_UI = $(BIN_DIR)/vtl_ui

CC := gcc
CFLAGS += -g -Wall -Wextra -Wpedantic \
          -Wformat=2 -Wno-unused-parameter -Wshadow \
          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
          -Wredundant-decls -Wnested-externs -Wmissing-include-dirs
CFLAGS += -Wnull-dereference -Wjump-misses-init -Wlogical-op -fcommon
CFLAGS += -O2
CFLAGS += -I$(LIBBPF_DIR)/build/usr/include/ -g
CFLAGS += -I$(ROOT_DIR)/include/headers/

ARFLAGS = rcs
LDFLAGS = '-Wl,-L$(LIBBPF_DIR),--allow-multiple-definition'

LIBS := -l:libbpf.a -lelf
LIBS += -lpcap
LIBS += -lz
LIBS += -lm

all: build
	@echo "Build finished."

.PHONY: clean
clean: clean-bin clean-modules clean-bpfprog clean-libtools

build: build-libtools build-bpfprog build-modules

build-libtools: $(LIBTOOLS)
	@echo "Build libtools finished."

build-bpfprog: $(BPFPROG_DIR)
	@echo "Build bpf programs finished."

build-modules: $(MODULES) build-libvtl build-vtlui
	@echo "Build vtl modules finished."

build-libvtl: $(STATIC_LIBVTL)
	@echo "Build static libvtl finished."

build-vtlui: $(VTL_UI)
	@echo "Build vtl ui finished."

hooker-daemon: hooker.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)


clean-bin: $(BIN_CLEAN)
	@echo "Clean bin finished."

clean-modules: $(MODULES_CLEAN)
	@echo "Clean modules finished."

clean-bpfprog: $(BPFPROG_CLEAN)
	@echo "Clean bpf programs finished."

clean-libtools: $(LIBTOOLS_CLEAN)
	@echo "Clean libtools finished."


$(LIBTOOLS) $(BPFPROG_DIR) $(MODULES): force
	$(MAKE) -C $@

$(STATIC_LIBVTL): $(LIBVTL_OBJS)
	$(AR) $(ARFLAGS) $@ $?

$(VTL_UI): $(SRC_DIR)/ui/vtl_ui.o $(LIBVTL_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LIBVTL_OBJS) $< $(LIBS)

$(LIBTOOLS_CLEAN) $(BPFPROG_CLEAN) $(MODULES_CLEAN):
	$(MAKE) -C $(subst _clean,,$@) clean

$(BIN_CLEAN):
	$(RM) $(STATIC_LIBVTL) $(VTL_UI)

force :;
