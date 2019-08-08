APP_PLATFORM := android-16

# 添加vector map等c++支持
APP_STL :=c++_shared

APP_CFLAGS :=-std=c++11

APP_ABI = armeabi-v7a

APP_CFLAGS += -Wno-error=format-security
