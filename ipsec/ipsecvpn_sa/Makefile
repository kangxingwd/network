# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2010-2014 Intel Corporation

# binary name
APP = ipsecvpn_sa

# all source are stored in SRCS-y
SRCS-y := log.c ipsecvpn_sa.c main.c

LIBDIR = /lib64/

EXTRA_CFLAGS += -I$(shell pwd)/../dep/include/ -I$(shell pwd)/../dep/include/libnl/ -Wmissing-prototypes -O0 -g
EXTRA_LDFLAGS += -L$(shell pwd)/../dep/lib/libnl/lib -lpthread -lnl-3 -lnl-xfrm-3

$(info CFLAGS:   ${CFLAGS})
$(info LDFLAGS:  ${LDFLAGS})

# Build using pkg-config variables if possible
ifeq ($(shell pkg-config --exists libdpdk && echo 0),0)

all: shared
.PHONY: shared static
shared: build/$(APP)-shared
	ln -sf $(APP)-shared build/$(APP)
static: build/$(APP)-static
	ln -sf $(APP)-static build/$(APP)

PKGCONF=pkg-config --define-prefix

PC_FILE := $(shell $(PKGCONF) --path libdpdk)
CFLAGS += -O3 $(shell $(PKGCONF) --cflags libdpdk)
CFLAGS += -DALLOW_EXPERIMENTAL_API
LDFLAGS_SHARED = $(shell $(PKGCONF) --libs libdpdk)
LDFLAGS_STATIC = -Wl,-Bstatic $(shell $(PKGCONF) --static --libs libdpdk)

build/$(APP)-shared: $(SRCS-y) Makefile $(PC_FILE) | build
	$(CC) $(CFLAGS) $(SRCS-y) -o $@ $(LDFLAGS) $(LDFLAGS_SHARED)

build/$(APP)-static: $(SRCS-y) Makefile $(PC_FILE) | build
	$(CC) $(CFLAGS) $(SRCS-y) -o $@ $(LDFLAGS) $(LDFLAGS_STATIC)

build:
	@mkdir -p $@

.PHONY: clean
clean:
	rm -f build/$(APP) build/$(APP)-static build/$(APP)-shared
	test -d build && rmdir -p build || true

else # Build using legacy build system

ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, detect a build directory, by looking for a path with a .config
RTE_TARGET ?= $(notdir $(abspath $(dir $(firstword $(wildcard $(RTE_SDK)/*/.config)))))
$(info RTE_TARGET:  ${RTE_TARGET})
include $(RTE_SDK)/mk/rte.vars.mk

ifneq ($(CONFIG_RTE_EXEC_ENV_LINUX),y)
$(error This application can only operate in a linux environment, \
please change the definition of the RTE_TARGET environment variable)
endif

CFLAGS += -O3
CFLAGS += -DALLOW_EXPERIMENTAL_API
CFLAGS += $(WERROR_FLAGS)

include $(RTE_SDK)/mk/rte.extapp.mk
endif
