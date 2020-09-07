# c-template project (https://gitlab.com/arturmadrzak/c-template)
# Copyright (c) 2020 Artur Mądrzak <artur@madrzak.eu>

MODULES = $(patsubst ../%.c,%,$(wildcard ../*.c))
MOCKS = $(patsubst %.c,%.o,$(wildcard mock_*.c))
TESTS = $(patsubst %.c,%,$(filter-out $(MOCKS:.o=.c),$(wildcard *.c)))
FUNCTION_TESTS = $(filter-out $(MODULES),$(TESTS))
MODULE_TESTS = $(filter-out $(FUNCTION_TESTS),$(TESTS))

CFLAGS = -I.. -Wall -fPIC -MMD -DNDEBUG
LDFLAGS = -shared --whole-archive


.PHONY: all clean $(TESTS)

all: $(TESTS)

$(TESTS): %: %.so
	@cgreen-runner $<

# Template for rule linking module test (test/module.c)
# 1 - module
define MODULE_TEST_template =
$(1).so: $(1).o ../$(1).o $(filter-out mock_$(1).o,$(MOCKS))
	$$(LD) $$(LDFLAGS) -o $$@ $$^ $$(LOADLIBES) $$(LDLIBS)
endef

$(foreach module_test,$\
	$(MODULE_TESTS),$\
	$(eval $(call MODULE_TEST_template,$(module_test))))

# Template for rule linking function test for specific module
# (test/module.function.c)
# 1 - module
# 2 - function
define FUNCTION_TEST_template =
$(1).$(2).so: $(1).$(2).o ../$(1).o $(filter-out mock_$(1).o,$(MOCKS))
	$$(LD) $$(LDFLAGS) -o $$@ $$^ $$(LOADLIBES) $$(LDLIBS)
endef

$(foreach function_test,$\
	$(FUNCTION_TESTS),$\
	$(eval $(call FUNCTION_TEST_template,$\
			$(word 1,$(subst ., ,$(function_test))),$\
			$(word 2,$(subst ., ,$(function_test))))))

clean:
	rm -f *.o
	rm -f *.d
	rm -f *.so

include $(wildcard *.d)
