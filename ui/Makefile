BIN := vtl_ui
OBJS := ${BIN:=.o}

DEP_DIR = deps
COMMON_DIR = ../include/common

DEPS := $(patsubst %,$(DEP_DIR)/%.d,$(basename $(OBJS)))
# compilers (at least gcc and clang) don't create the subdirectories automatically
$(shell mkdir -p $(dir $(DEPS)) >/dev/null)

include $(COMMON_DIR)/common.mk
