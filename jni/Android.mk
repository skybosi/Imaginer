#获取当前目录
LOCAL_PATH := $(call my-dir)
#清除一些变量
include $(CLEAR_VARS)
#要生成的库名
LOCAL_MODULE    := libImaginer
#指定平台
#LOCAL_ARM_MODE := arm
#需要引用的库
LOCAL_LDFLAGS  := -fPIC  -shared  -Wl -ldl
LOCAL_CPPFLAGS += -fexceptions
#-L$(SYSROOT)/usr/lib -lrt -luuid -pthread
#编译参数
LOCAL_CFLAGS := -Wall -O3 -enable-threads
#定义宏
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include
#LOCAL_C_INCLUDES := ../include
#LOCAL_SRC_FILES := ../include/ipoint.cpp ../include/Rbmp.cpp
#库对应的源文件
MY_CPP_LIST := $(wildcard $(LOCAL_PATH)/../include/*.cpp)
LOCAL_SRC_FILES := $(MY_CPP_LIST:$(LOCAL_PATH)/%=%)
#生成共享库
include $(BUILD_SHARED_LIBRARY)
