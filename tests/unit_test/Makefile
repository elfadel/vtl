USER_TARGETS := unit_test

LIBBPF_DIR = ../../include/libbpf/src
COMMON_DIR = ../../include/common
LAUNCHER_DIR = ../../launcher

EXTRA_DEPS := $(LAUNCHER_DIR)/launcher.h

DEPS_OBJS = $(LAUNCHER_DIR)/launcher.o

include $(COMMON_DIR)/common.mk

LIBS += -lpcap