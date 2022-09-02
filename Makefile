export MAPFILE := GALE01r2.map
export INCLUDE := include

CODES := dashback

ucf: $(CODES)

define make_gecko_rule
$1:
	+@$(MAKE) -f cpp2gecko/Makefile gecko SRCDIR=src/$1 BUILDDIR=build/$1
endef

$(foreach code, $(CODES), $(eval $(call make_gecko_rule,$(code))))