BIN := hooker_user
OBJS := ${BIN:=.o}

DEP_DIR = deps
COMMON_DIR = ../include/common

DEPS := $(patsubst %,$(DEP_DIR)/%.d,$(basename $(OBJS)))

$(shell mkdir -p $(dir $(DEPS)) > /dev/null)

include $(COMMON_DIR)/common.mk