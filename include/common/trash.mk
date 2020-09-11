# Common Makefile parts for BPF-building with libbpf
# --------------------------------------------------
# SPDX-License-Identifier: (GPL-2.0 OR BSD-2-Clause)


LLC ?= llc
CLANG ?= clang
CC ?= gcc

USER_SRC := ${USER_TARGETS:=.c}
USER_OBJ := ${USER_SRC:.c=.o}

# Expect this is defined by including Makefile, but define if not
COMMON_DIR ?= ../common/
LIBBPF_DIR ?= ../libbpf/src/


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


all: $(USER_TARGETS) 

.PHONY: clean $(CLANG) $(LLC)

## TODO: peut-être ajouter : mrproper ??
## TODO: add clean USER_DEPS
clean:
	rm -rf $(LIBBPF_DIR)/build
	$(MAKE) -C $(LIBBPF_DIR) clean
	$(MAKE) -C $(COMMON_DIR) clean
	rm -f $(USER_TARGETS) $(USER_OBJ)
	rm -f *.ll
	rm -f *~


# For build dependency on this file, if it gets updated
COMMON_MK = $(COMMON_DIR)/common.mk

# Create dependency: detect if C-file change and touch H-file, to trigger
# target $(COMMON_OBJS)
$(COMMON_H): %.h: %.c
	touch $@

# TODO: Penser à utiliser cette méthode pour la compilation des protocoles de transport
# Detect if any of common obj changed and create dependency on .h-files
$(COMMON_OBJS): %.o: %.h
	make -C $(COMMON_DIR)

$(USER_TARGETS): %: %.c Makefile $(COMMON_MK) $(COMMON_OBJS) $(DEPS_OBJS) $(KERN_USER_H) $(EXTRA_DEPS)
	$(CC) -Wall $(CFLAGS) $(USER_DEPS) $(LDFLAGS) -o $@ $(COMMON_OBJS) $(DEPS_OBJS) \
	 $< $(LIBS)


