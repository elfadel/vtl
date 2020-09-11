OBJS := util.o
OBJS += tc_user_helpers.o
OBJS += xdp_user_helpers.o
OBJS += xsk_user_helpers.o


DEP_DIR = deps
COMMON_DIR = .

DEPS := $(patsubst %,$(DEP_DIR)/%.d,$(basename $(OBJS)))

# compilers (at least gcc and clang) don't create the subdirectories automatically
$(shell mkdir -p $(dir $(DEPS)) >/dev/null)

include $(COMMON_DIR)/special_common.mk
