ifndef LOAD_KIND
$(error LOAD_KIND is not set! (are you not using exlaunch.sh?))
endif

.PHONY: clean all

# Forward defines.
export EXL_CFLAGS	:= -DEXL_LOAD_KIND=$(LOAD_KIND) -DEXL_LOAD_KIND_ENUM=$(LOAD_KIND_ENUM) -DEXL_PROGRAM_ID=0x$(PROGRAM_ID) -DNNSDK

include $(MK_PATH)/common.mk
