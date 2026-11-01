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
SOURCES := source source/platform source/game
INCLUDES := include
APP := super-mario-63-enhanced-wii

CFLAGS = -g -O2 -Wall -Wextra -Werror $(MACHDEP) $(INCLUDE) -std=gnu11
CXXFLAGS = -g -O2 -Wall -Wextra -Werror $(MACHDEP) $(INCLUDE) -std=gnu++17
LDFLAGS = -g $(MACHDEP) -Wl,-Map,$(OUTPUT).elf.map
LIBS := -lwiiuse -lbte -logc -lm

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export DEPSDIR := $(CURDIR)/$(BUILD)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) -I$(LIBOGC_INC)
export LIBPATHS := -L$(LIBOGC_LIB)
CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
export OFILES := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
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
	@mkdir -p dist/apps/$(APP)
	cp boot.dol packaging/meta.xml dist/apps/$(APP)/

clean:
	rm -rf $(BUILD) dist boot.dol boot.elf boot.elf.map
else
all: $(OUTPUT).dol
$(OUTPUT).dol: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)
-include $(DEPSDIR)/*.d
endif
