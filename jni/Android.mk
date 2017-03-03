#获取当前目录
LOCAL_PATH := $(call my-dir)
#清除一些变量
include $(CLEAR_VARS)
#要生成的库名
LOCAL_MODULE    := libImaginer
#指定平台
#LOCAL_ARM_MODE := arm
#需要引用的库
LOCAL_LDFLAGS  := -fPIC -shared  -Wl -ldl -m
LOCAL_CPPFLAGS += -fexceptions -frtti
#-L$(SYSROOT)/usr/lib -lrt -luuid -pthread
#编译参数
LOCAL_CFLAGS := -Wall -O3 -enable-threads
#定义宏
TOP_INCLUDE := $(LOCAL_PATH)/../include
LOCAL_C_INCLUDES := $(TOP_INCLUDE) \
	$(TOP_INCLUDE)/BaseData        \
	$(TOP_INCLUDE)/Commons         \
	$(TOP_INCLUDE)/DPC             \
	$(TOP_INCLUDE)/DPer            \
	$(TOP_INCLUDE)/MetaData        \
	$(TOP_INCLUDE)/Utils
$(warning "include" $(TOP_INCLUDE) $(LOCAL_C_INCLUDES))
TOP_SRC := $(LOCAL_PATH)/../src
#库对应的源文件
MY_CPP_LIST := $(wildcard $(TOP_SRC)/*/*.cpp)
$(warning "source" $(MY_CPP_LIST))
LOCAL_SRC_FILES := $(MY_CPP_LIST:$(LOCAL_PATH)/%=%)

#生成共享库
include $(BUILD_SHARED_LIBRARY)