ifndef REVISION
REVISION := 1.02
endif

ifeq ($(REVISION), 1.00)
export MAPFILE := GALE01r0.map
else ifeq ($(REVISION), 1.01)
export MAPFILE := GALE01r1.map
else ifeq ($(REVISION), 1.02)
export MAPFILE := GALE01r2.map
else ifeq ($(REVISION), PAL)
export MAPFILE := GALP01.map
else
$(error Unsupported Melee revision "$(REVISION)")
endif

export EXTRA_PPFLAGS := -Iinclude -DREVISION=\'$(REVISION)\'

CODES := $(shell find src -maxdepth 1 -type d | sed -rn 's/src\/(.*)/\1/p')

ucf: $(CODES)

define make_gecko_rule
.PHONY: $1
$1:
	+@$(MAKE) -f cpp2gecko/Makefile SRCDIR=src/$1 BUILDDIR=build/$(REVISION)/$1
endef

$(foreach code, $(CODES), $(eval $(call make_gecko_rule,$(code))))