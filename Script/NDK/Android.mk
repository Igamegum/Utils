#########################
LOCAL_PATH := $(call my-dir)



include $(CLEAR_VARS)
LOCAL_MODULE    := shared_crypto
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libcrypto.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := shared_ssl
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libssl.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := shared_json
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libjsoncpp.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := thinkjoy_authorization
LOCAL_SHARED_LIBRARIES :=  shared_crypto shared_ssl shared_json


MY_CPP_LIST := $(wildcard $(LOCAL_PATH)/src/*.cpp)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/src/util/*.cpp)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/src/thirdpartylibraries/tinyxml2/*.cpp)
MY_CPP_LIST += $(wildcard $(LOCAL_PATH)/src/authorization/*.cpp)


$(warning $(MY_CPP_LIST))
LOCAL_SRC_FILES := $(MY_CPP_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include    
LOCAL_C_INCLUDES += $(shell ls -FR $(LOCAL_C_INCLUDES) | grep $(LOCAL_PATH)/$ | sed "s/:/ /g" )


LOCAL_LDLIBS += -llog
LOCAL_LDLIBS += -landroid
include $(BUILD_SHARED_LIBRARY)


