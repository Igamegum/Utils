APP_PLATFORM := android-14
#APP_ABI :=  armeabi-v7a 
#APP_ABI :=  arm64-v8a
APP_ABI :=  armeabi-v7a  arm64-v8a
APP_STL :=gnustl_static
#APP_STL :=gnustl_shared
NDK_TOOLCHAIN_VERSION :=4.9
#NDK_TOOLCHAIN_VERSION := clang3.6-obfuscator
#LOCAL_CPPFLAGS += -D__cplusplus=201103L
APP_CPPFLAGS += -std=c++11 

