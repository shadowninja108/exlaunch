ifndef LOAD_KIND
$(error LOAD_KIND is not set! (are you not using exlaunch.sh?))
endif

.PHONY: clean all

# Forward defines.
export EXL_CFLAGS	:= -DEXL_LOAD_KIND=$(LOAD_KIND)	-DEXL_PROGRAM_ID=0x$(PROGRAM_ID)

include $(MK_PATH)/common.mk
