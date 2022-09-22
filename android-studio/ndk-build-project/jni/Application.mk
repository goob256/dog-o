
# Uncomment this if you're using STL in your project
# See CPLUSPLUS-SUPPORT.html in the NDK documentation for more information
APP_STL := c++_shared

APP_ABI := armeabi-v7a arm64-v8a x86 x86_64

# Min SDK level
APP_PLATFORM=android-23

# goobliata
APP_CPPFLAGS += -fexceptions -frtti

APP_OPTIM := release
