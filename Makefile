### Makefile for project ###
PROJECT=contention-queue
DEFINES=REPETITIONS=10000 PRODUCERS=3
OBJ_DIR=build
SRC_DIR=src

### Compiler and linker settings ###
CC=$(if $(shell which colorgcc),colorgcc,gcc)
LD := gcc
CFLAGS := -Wall -Wextra -pedantic
LDLIBS := pthread stdc++ 

### Generic make variables ###
DEF := $(filter-out %DEBUG,$(DEFINES)) $(if $(filter DEBUG,$(DEFINES)),DEBUG,NDEBUG)
SRC := $(shell find $(SRC_DIR) -type f -regextype posix-extended -regex ".+\.cpp")
HDR := $(shell find $(SRC_DIR) -type f -regextype posix-extended -regex ".+\.h")
ALL := $(SRC) $(HDR) Makefile LICENSE README.md


### Make targets ###
.PHONY: $(PROJECT) all debug clean realclean todo
all: $(PROJECT)

debug: DEF += DEBUG

debug: all

define compile_target
$(OBJ_DIR)/$(2): $(1) $(HDR)
	-@mkdir -p $$(@D)
	$$(CC) -x c++ -std=gnu++11 $$(CFLAGS) $$(if $$(filter DEBUG,$$(DEF)),-g,) $$(addprefix -D,$$(DEF:-D%=D)) -o $$@ -c $$<
OBJ += $(OBJ_DIR)/$(2)
endef

$(foreach file,$(SRC),$(eval $(call compile_target,$(file),$(notdir $(file:%.cpp=%.o)))))

$(PROJECT): $(OBJ)
	$(LD) -o $@ $^ $(addprefix -l,$(LDLIBS:-l%=%))

clean:
	-$(RM) $(OBJ)

realclean: clean
	-$(RM) $(PROJECT)

todo:
	-@for file in $(ALL:Makefile=); do \
		fgrep -H -e TODO -e FIXME $$file; \
	done; true
