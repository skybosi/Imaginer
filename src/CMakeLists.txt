cmake_minimum_required(VERSION 2.8)
include(../Imaginer.cmake)
# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)
aux_source_directory(./MetaData DIR_LIB_SRCS)
aux_source_directory(./Utils DIR_LIB_SRCS)
aux_source_directory(./Dper DIR_LIB_SRCS)
aux_source_directory(./DPC DIR_LIB_SRCS)
## 指定include 路径
set(TOP_INCLUDE "${PROJECT_SOURCE_DIR}/../include")
include_directories(${TOP_INCLUDE}
	${TOP_INCLUDE}/BaseData 
	${TOP_INCLUDE}/Commons
	${TOP_INCLUDE}/DPC
	${TOP_INCLUDE}/DPer
	${TOP_INCLUDE}/MetaData
	${TOP_INCLUDE}/Utils )
# 生成动态链接库
add_library (Imaginer SHARED ${DIR_LIB_SRCS})
# 生成静态链接库
#add_library (Imaginer STATIC ${DIR_LIB_SRCS})
