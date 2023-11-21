# Define paths.
PWD := $(shell pwd)
MISC_PATH := $(PWD)/misc
MK_PATH := $(MISC_PATH)/mk
SCRIPTS_PATH := $(MISC_PATH)/scripts
SPECS_PATH := $(MISC_PATH)/specs
NPDM_JSON_PATH := $(MISC_PATH)/npdm-json

# To configure exlaunch, edit config.mk.
include $(PWD)/config.mk

# Define common variables.
NAME := $(shell basename $(PWD))
OUT := $(PWD)/deploy
SD_OUT := atmosphere/contents/$(PROGRAM_ID)/exefs


# Set load kind specific variables.
ifeq ($(LOAD_KIND), Module)
    LOAD_KIND_ENUM := 2
    BINARY_NAME := subsdk9 # TODO: support subsdkX?
    SPECS_NAME := module.specs
    MK_NAME := module.mk
else ifeq ($(LOAD_KIND), AsRtld)
    LOAD_KIND_ENUM := 1
    BINARY_NAME := rtld
    SPECS_NAME := as_rtld.specs
    MK_NAME := as_rtld.mk
else
    $(error LOAD_KIND is invalid, please check config.mk)
endif

ifeq ($(FAKEHEAP_SIZE),)
    DEXL_FAKEHEAP := -DEXL_FAKEHEAP_SIZE=0x1000
else
    DEXL_FAKEHEAP := -DEXL_FAKEHEAP_SIZE=$(FAKEHEAP_SIZE) -DEXL_USE_FAKEHEAP
endif

.PHONY: clean all

# Built internal C flags variable.
EXL_CFLAGS   := $(C_FLAGS) \
                $(DEXL_FAKEHEAP) \
                -DEXL_LOAD_KIND=$(LOAD_KIND) \
                -DEXL_LOAD_KIND_ENUM=$(LOAD_KIND_ENUM) \
                -DEXL_PROGRAM_ID=0x$(PROGRAM_ID) \
                -DEXL_MODULE_NAME=\"$(MODULE_NAME)\" \
                -DEXL_JIT_SIZE=$(JIT_SIZE) \
                -DEXL_INLINE_POOL_SIZE=$(INLINE_POOL_SIZE) \

ifeq ($(DEBUG), 1)
    EXL_CFLAGS := $(EXL_CFLAGS) -DEXL_DEBUG
endif

ifeq ($(SUPPORTS_REBOOTPAYLOAD), 1)
    EXL_CFLAGS := $(EXL_CFLAGS) -DEXL_SUPPORTS_REBOOTPAYLOAD
endif

EXL_CXXFLAGS := $(CXX_FLAGS)

# Export all of our variables to sub-makes and sub-processes.
export

include $(MK_PATH)/$(MK_NAME)
include $(MK_PATH)/common.mk
include $(MK_PATH)/deploy.mk
include $(MK_PATH)/npdm.mk
