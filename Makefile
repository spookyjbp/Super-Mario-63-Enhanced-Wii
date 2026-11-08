# Uses the official devkitPPC/libogc Wii rules. Supports C and C++ modules.
.SUFFIXES:
.SECONDARY:
.DEFAULT_GOAL := all

ifeq ($(strip $(DEVKITPPC)),)
$(error DEVKITPPC is unset; install devkitPro wii-dev or use devkitpro/devkitppc)
endif
include $(DEVKITPPC)/wii_rules

TARGET := boot
BUILD := build
SOURCES := source source/platform source/game source/render source/assets
INCLUDES := include assets/generated
DATA := assets/generated
APP := super-mario-63-enhanced-wii

CFLAGS = -g -O2 -Wall -Wextra -Werror $(MACHDEP) $(INCLUDE) -std=gnu11
CXXFLAGS = -g -O2 -Wall -Wextra -Werror $(MACHDEP) $(INCLUDE) -std=gnu++17
LDFLAGS = -g $(MACHDEP) -Wl,-Map,$(OUTPUT).elf.map
LIBS := -lfat -lwiiuse -lbte -lasnd -logc -lm
# make DEV=1: the developer achievement tests (pause menu > Achievements;
# saves to sd:/sm63wii/devsave.dat instead of save.dat)
ifeq ($(DEV),1)
CFLAGS += -DSM63_DEV
endif

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export DEPSDIR := $(CURDIR)/$(BUILD)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) $(CURDIR)/$(DATA)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) -I$(LIBOGC_INC) -I$(CURDIR)/$(BUILD)
export LIBPATHS := -L$(LIBOGC_LIB)
CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
BINFILES := $(notdir $(wildcard $(DATA)/*.bin))
export OFILES_SOURCES := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
export OFILES := $(OFILES_SOURCES) $(addsuffix .o,$(BINFILES))
export HFILES := $(addsuffix .h,$(subst .,_,$(BINFILES)))
ifeq ($(strip $(CPPFILES)),)
export LD := $(CC)
else
export LD := $(CXX)
endif

.PHONY: all $(BUILD) clean package
all: $(BUILD)
$(BUILD):
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

package: all
	@mkdir -p dist/apps/$(APP)/data
	cp boot.dol packaging/meta.xml dist/apps/$(APP)/
	cp assets/data/*.bin dist/apps/$(APP)/data/

clean:
	rm -rf $(BUILD) dist boot.dol boot.elf boot.elf.map
else
all: $(OUTPUT).dol
$(OUTPUT).dol: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)
$(OFILES_SOURCES): $(HFILES)
%.bin.o %_bin.h: %.bin
	@$(bin2o)
-include $(DEPSDIR)/*.d
endif
