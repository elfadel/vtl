BPF_TARGETS := egress_test_graft.o
BPF_TARGETS += ingress_test_graft.o
BPF_TARGETS += hooker_test_progs.o

INCLUDE_BPF = include
INCLUDE_VTL = ../include
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/source

ARCH := $(subst x86_64,x86,$(shell arch))

LLC ?= llc
CLANG ?= clang

# Activated time cmd if you want to compute
# KTFs/Grafts compilation delay
#TIME ?= time 

CFLAGS ?= -I./$(INCLUDE_BPF) 
CFLAGS += -I$(KERNEL_DIR)/
CFLAGS += -I../deps/

EXTRA_DEPS := $(INCLUDE_VTL)/vtl.h 

all: $(BPF_TARGETS)

.PHONY: clean $(CLANG) $(LLC)

clean:
	rm -f $(BPF_TARGETS)
	rm -f *.ll
	rm -f *~

$(BPF_TARGETS): %.o: %.c Makefile $(EXTRA_DEPS)
	$(CLANG) -S 					\
		-target bpf 				\
		-D __BPF_TRACING__ 			\
		$(CFLAGS) 				\
		-Wall 					\
		-Wno-unused-value 			\
		-Wno-pointer-sign 			\
		-Wno-compare-distinct-pointer-types 	\
		-Werror 				\
		-O2 -emit-llvm -c -g -o ${@:.o=.ll} $<
	$(TIME) $(LLC) -march=bpf -filetype=obj -o $@ ${@:.o=.ll}
