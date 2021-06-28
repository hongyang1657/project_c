LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

include $(call all-makefiles-under,$(LOCAL_PATH))



LOCAL_ARM_MODE := arm




LOCAL_SRC_FILES := src/decode.c \
	src/encode.c \
	src/kiss_fft.c \
	src/common.c \
	src/rscode.c 

	

	
LOCAL_C_INCLUDES := \
	~/exdroid/Trunk/eldk/includes \
	$(LOCAL_PATH)/include
	

#	$(LOCAL_PATH)/source/common
#	$(LOCAL_PATH)/include 

#LOCAL_SHARED_LIBRARIES := libc
LOCAL_SHARED_LIBRARIES := \
	libui libcutils libutils libc

			
LOCAL_CFLAGS += -DCTOOL=1 -DLLVL=1 -DFIXED_POINT=16 -DADD_SYNC_TONE



LOCAL_MODULE_TAGS := eng
 
#LOCAL_CFLAGS += -O3
#LOCAL_CFLAGS += -march=armv6j

LOCAL_MODULE:= libADT


include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)

