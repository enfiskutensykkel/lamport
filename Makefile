### Makefile for project ###
PROJECT=lamport
DEFINES=REPETITIONS=20000000 PRODUCERS=3 QUEUE_SIZE=512
OBJ_DIR=build
SRC_DIR=src

### Compiler and linker settings ###
CC=$(if $(shell which colorgcc),colorgcc,gcc)
LD := gcc
CFLAGS := -Wall -Wextra -pedantic
LDLIBS := pthread stdc++ rt

### Generic make variables ###
DEF := $(filter-out %DEBUG,$(DEFINES)) 
SRC := $(wildcard $(SRC_DIR)/*.cpp)
HDR := $(wildcard $(SRC_DIR)/*.h)
ALL := $(SRC) $(HDR) Makefile README.md


### Make targets ###
.PHONY: $(PROJECT) all debug clean distclean todo tar
all: $(PROJECT)

debug: DEF += DEBUG

debug: all

define compile_target
$(OBJ_DIR)/$(2): $(1) $(HDR)
	-@mkdir -p $$(@D)
	$$(CC) -x c++ -std=gnu++11 $$(CFLAGS) $$(if $$(filter DEBUG,$$(DEF)),-g,-DNDEBUG) $$(addprefix -D,$$(DEF:-D%=D)) -o $$@ -c $$<
OBJ += $(OBJ_DIR)/$(2)
endef

$(foreach file,$(SRC),$(eval $(call compile_target,$(file),$(notdir $(file:%.cpp=%.o)))))

$(PROJECT): $(OBJ)
	$(LD) -o $@ $^ $(addprefix -l,$(LDLIBS:-l%=%))

clean:
	-$(RM) $(OBJ)

distclean: clean
	-$(RM) $(PROJECT)

todo:
	-@for file in $(ALL:Makefile=); do \
		fgrep -H -e TODO -e FIXME $$file; \
	done; true

tar:
	ln -s . $(PROJECT)
	tar -cz -f $(PROJECT)-$(shell date +%Y-%m-%d_%H-%M-%S).tar.gz $(addprefix $(PROJECT)/,$(ALL))
	-$(RM) $(PROJECT)
