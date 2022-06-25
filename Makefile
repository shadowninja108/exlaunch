ifndef LOAD_KIND
$(error LOAD_KIND is not set! (are you not using exlaunch.sh?))
endif

.PHONY: clean all

# Built internal C flags variable.
export EXL_CFLAGS	:= $(C_FLAGS) -DEXL_LOAD_KIND=$(LOAD_KIND) -DEXL_LOAD_KIND_ENUM=$(LOAD_KIND_ENUM) -DEXL_PROGRAM_ID=0x$(PROGRAM_ID)
export EXL_CXXFLAGS	:= $(CXX_FLAGS)

include $(MK_PATH)/$(MK_NAME)
include $(MK_PATH)/common.mk
