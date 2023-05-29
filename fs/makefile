include settings.mk

################################################################################

filename = $(1).out

define compile_bin
gcc $(CFLAGS) -o "$@" $^ $(IDIRS:%=-I%) $(LIBDIRS:%=-L%) $(RUNDIRS:%=-Wl,-rpath,%) $(LIBS:%=-l%)
endef

define compile_objs
gcc $(CFLAGS) -c -o "$@" $< $(IDIRS:%=-I%)
endef

################################################################################

# Set prerrequisites
SRCS_C += $(shell find src/ -iname "*.c")
SRCS_H += $(shell find include/ -iname "*.h")
DEPS = $(foreach SHL,$(SHARED_LIBPATHS),$(SHL:%=%/bin/lib$(notdir $(SHL)).so)) \
	$(foreach STL,$(STATIC_LIBPATHS),$(STL:%=%/bin/lib$(notdir $(STL)).a))

# Set header paths to (-I)nclude
IDIRS += $(addsuffix /include,$(SHARED_LIBPATHS) $(STATIC_LIBPATHS) .)

# Set library paths to (-L)ook
LIBDIRS = $(addsuffix /bin,$(SHARED_LIBPATHS) $(STATIC_LIBPATHS))

# Set shared library paths to be found in runtime (-rpath)
RUNDIRS = $(SHARED_LIBPATHS:%=$(shell cd . && pwd)/%/bin)

# Set intermediate objects
OBJS = $(patsubst src/%.c,obj/%.o,$(SRCS_C))

# Set binary target
BIN = bin/$(call filename,$(shell cd . && pwd | xargs basename))

.PHONY: all
all: CFLAGS = $(CDEBUG)
all: $(BIN)

.PHONY: release
release: CFLAGS = $(CRELEASE)
release: $(BIN)

.PHONY: clean
clean:
	-rm -rfv obj bin

.PHONY: watch
watch:
	@test $(shell which entr) || entr
	while sleep 0.1; do \
		find src/ include/ | entr -d make all --no-print-directory; \
	done

$(BIN): $(OBJS) | $(dir $(BIN))
	$(call compile_bin)

obj/%.o: src/%.c $(SRCS_H) $(DEPS) | $(dir $(OBJS))
	$(call compile_objs)

.SECONDEXPANSION:
$(DEPS): $$(shell find $$(patsubst %bin/,%src/,$$(dir $$@)) -iname "*.c") \
	$$(shell find $$(patsubst %bin/,%include/,$$(dir $$@)) -iname "*.h")
	make --no-print-directory -C $(patsubst %bin/,%,$(dir $@))

$(sort $(dir $(BIN) $(OBJS))):
	mkdir -pv $@


################################################################################

.PHONY: start
start: all
	valgrind --tool=none ./$(BIN) $(ARGS)

.PHONY: daemon
daemon:
	@test $(shell which entr) || entr
	while sleep 0.1; do \
		find src/ include/ | entr -d make start --no-print-directory; \
	done

.PHONY: memcheck
memcheck: all
	valgrind --leak-check=full $(MEMCHECK_FLAGS) ./$(BIN) $(ARGS)

.PHONY: helgrind
helgrind: all
	valgrind --tool=helgrind $(HELGRIND_FLAGS) ./$(BIN) $(ARGS)

obj/%.o: src/%.c tests/%.c $(SRCS_H) $(DEPS) | $(dir $(OBJS))
	gcc $(CFLAGS) -c -o "$@" $< $(IDIRS:%=-I%)

.PHONY: build-check
build-check: CFLAGS = $(CDEBUG)
build-check: OBJS += $(patsubst tests/%.c,obj/%.o,$(shell find tests/ -iname "*.c"))
build-check: OBJS += $(patsubst src/%.c,obj/%.o,$(shell find src/ -iname "*.c"))
build-check: DEPS += $(shell find src/ -iname "*.c" -not -name "main.c")
build-check: LIBS = static commons check pthread m rt subunit
build-check: $(OBJS) | $(dir $(BIN))
	@echo "\033[1;32mCFLAGS: $(CFLAGS)\nDEPS: $(DEPS)\nOBJS: $(OBJS)\nLIBS: $(LIBS)\nIDIRS: $(IDIRS)\nLIBDIRS: $(LIBDIRS)\033[0m"
	gcc $(CFLAGS) -o bin/check tests/check.c $(DEPS) $(IDIRS:%=-I%) $(LIBDIRS:%=-L%) $(LIBS:%=-l%)

.PHONY: check
check: CFLAGS = $(CDEBUG)
check: OBJS += $(patsubst tests/%.c,obj/%.o,$(shell find tests/ -iname "*.c"))
check: OBJS += $(patsubst src/%.c,obj/%.o,$(shell find src/ -iname "*.c"))
check: DEPS += $(shell find src/ -iname "*.c" -not -name "main.c")
check: LIBS = static commons check pthread m rt subunit
check: $(OBJS) | $(dir $(BIN))
	@echo "\033[1;32mCFLAGS: $(CFLAGS)\nDEPS: $(DEPS)\nOBJS: $(OBJS)\nLIBS: $(LIBS)\nIDIRS: $(IDIRS)\nLIBDIRS: $(LIBDIRS)\033[0m"
	gcc $(CFLAGS) -o bin/check tests/check.c $(DEPS) $(IDIRS:%=-I%) $(LIBDIRS:%=-L%) $(LIBS:%=-l%)
	./bin/check || true

.PHONY: cunit
cunit: CFLAGS = $(CDEBUG)
cunit: OBJS += $(patsubst tests/%.c,obj/%.o,$(shell find tests/ -iname "*.c"))
check: OBJS += $(patsubst src/%.c,obj/%.o,$(shell find src/ -iname "*.c"))
cunit: LIBS = static commons cunit pthread m rt subunit
cunit: $(OBJS) | $(dir $(BIN))
	gcc $(CFLAGS) -o bin/cunit tests/cunit.c $(DEPS) $(IDIRS:%=-I%) $(LIBDIRS:%=-L%) $(LIBS:%=-l%)
	./bin/cunit