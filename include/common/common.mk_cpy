# Common Makefile parts for BPF-building with libbpf
# --------------------------------------------------
# SPDX-License-Identifier: (GPL-2.0 OR BSD-2-Clause)
#
# This file should be included from your Makefile like:
#  COMMON_DIR = ../common/
#  include $(COMMON_DIR)/common.mk
#
# It is expected that you define the variables:
#  XDP_TARGETS and USER_TARGETS
# as a space-separated list
#

# USER_TARGETS : prog in userspace
# KERN_TARGETS : prog in kernel space

LLC ?= llc
CLANG ?= clang
CC ?= gcc

#KERN_SRC = ${KERN_TARGETS:=.c}
#KERN_OBJ = ${KERN_SRC:.c=.o}
USER_SRC := ${USER_TARGETS:=.c}
USER_OBJ := ${USER_SRC:.c=.o}

# Expect this is defined by including Makefile, but define if not
COMMON_DIR ?= ../common/
LIBBPF_DIR ?= ../libbpf/src/


OBJECT_LIBBPF = $(LIBBPF_DIR)/libbpf.a

# Extend if including Makefile already added some
# TODO: find a better way to include common_objs
# $(COMMON_DIR)/params.o 
# $(COMMON_DIR)/sock_user_helpers.o
COMMON_OBJS +=  $(COMMON_DIR)/params.o
COMMON_OBJS +=  $(COMMON_DIR)/util_libbpf.o 
COMMON_OBJS +=	$(COMMON_DIR)/util_user_maps.o 
COMMON_OBJS +=	$(COMMON_DIR)/cgroup_helpers.o 
COMMON_OBJS +=  $(COMMON_DIR)/sock_user_helpers.o
COMMON_OBJS +=	$(COMMON_DIR)/tc_user_helpers.o 
COMMON_OBJS +=	$(COMMON_DIR)/xdp_user_helpers.o 
COMMON_OBJS +=	$(COMMON_DIR)/xsk_user_helpers.o
	       


## Create expansions for dependencies
COMMON_H := ${COMMON_OBJS:.o=.h}

## TODO: Utilisable ?
EXTRA_DEPS +=

# BPF-prog kern and userspace shares struct via header file:
## Intéressant comme approche...
# KERN_USER_H ?= $(wildcard common_kern_user.h)

## En-tête pour les fichiers sources
CFLAGS ?= -I$(LIBBPF_DIR)/build/usr/include/ -g
# Extra include for Ubuntu issue #44
CFLAGS += -I/usr/include/x86_64-linux-gnu
## Autre en-têtes 
CFLAGS += -I../headers/
## Mettre à disposition les fichiers sources de libbpf
## pour l'édition de liens
LDFLAGS ?= -L$(LIBBPF_DIR)


## USER_LIBS semble être très intéressant
LIBS = -l:libbpf.a -lelf $(USER_LIBS)

## llvm-check : quelques vérification au niveau du compilateur llvm
## $(USER_TARGETS) : compilation normale pour le fichier d'en-tête
## $(KERN_OBJ) : compilation des fichiers kern objets => indispensable
all: llvm-check $(USER_TARGETS) 

.PHONY: clean $(CLANG) $(LLC)

## TODO: peut-être ajouter : mrproper ??
##TODO: add clean USER_DEPS
clean:
	rm -rf $(LIBBPF_DIR)/build
	$(MAKE) -C $(LIBBPF_DIR) clean
	$(MAKE) -C $(COMMON_DIR) clean
	# $(MAKE) -C $(ADAPTER_DIR) clean
#	rm -f $(USER_TARGETS) $(XDP_OBJ) $(USER_OBJ) $(COPY_LOADER) $(COPY_STATS)
	rm -f $(USER_TARGETS) $(USER_OBJ)
	rm -f *.ll
	rm -f *~


# For build dependency on this file, if it gets updated
COMMON_MK = $(COMMON_DIR)/common.mk

llvm-check: $(CLANG) $(LLC)
	@for TOOL in $^ ; do \
		if [ ! $$(command -v $${TOOL} 2>/dev/null) ]; then \
			echo "*** ERROR: Cannot find tool $${TOOL}" ;\
			exit 1; \
		else true; fi; \
	done

# Compilation de la bibliothèque statique libbpf
$(OBJECT_LIBBPF):
	@if [ ! -d $(LIBBPF_DIR) ]; then \
		echo "Error: Need libbpf submodule"; \
		echo "May need to run git submodule update --init"; \
		exit 1; \
	else \
		cd $(LIBBPF_DIR) && $(MAKE) all; \
		mkdir -p build; DESTDIR=build $(MAKE) install_headers; \
	fi

# Create dependency: detect if C-file change and touch H-file, to trigger
# target $(COMMON_OBJS)
$(COMMON_H): %.h: %.c
	touch $@

# TODO: Penser à utiliser cette méthode pour la compilation des protocoles de transport
# Detect if any of common obj changed and create dependency on .h-files
$(COMMON_OBJS): %.o: %.h
	make -C $(COMMON_DIR)

$(USER_TARGETS): %: %.c $(OBJECT_LIBBPF) Makefile $(COMMON_MK) $(COMMON_OBJS) $(DEPS_OBJS) $(KERN_USER_H) $(EXTRA_DEPS)
	$(CC) -Wall $(CFLAGS) $(USER_DEPS) $(LDFLAGS) -o $@ $(COMMON_OBJS) $(DEPS_OBJS) \
	 $< $(LIBS)

# $(KERN_OBJ): %.o: %.c  Makefile $(COMMON_MK) $(KERN_USER_H) $(EXTRA_DEPS) $(KERN_DEPS)
# 	$(CLANG) -S \
# 	    -target bpf \
# 	    -D __BPF_TRACING__ \
# 	    $(CFLAGS) \
# 	    -Wall \
# 	    -Wno-unused-value \
# 	    -Wno-pointer-sign \
# 	    -Wno-compare-distinct-pointer-types \
# 	    -Werror \
# 	    -O2 -emit-llvm -c -g -o ${@:.o=.ll} $<
# 	$(LLC) -march=bpf -filetype=obj -o $@ ${@:.o=.ll}
