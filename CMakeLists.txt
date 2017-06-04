## Make 最低版本号要求
cmake_minimum_required (VERSION 2.8)
## 项目信息
project (Imager)
include(${PROJECT_SOURCE_DIR}/Imaginer.cmake)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	message("this operation platform is linux/unix")
	# 查找当前目录下的所有源文件
	# 添加 Imaginer 子目录
	add_subdirectory(src)
	# 指定生成目标
	add_executable(Imager main.cpp src/DPer/BmpPer.cpp)
	# 添加本工程链接库依赖
	target_link_libraries(Imager Imaginer)
else()
	message("this operation platform is windows")
	# 查找src目录下的所有源文件
	# 并将名称保存到 DIR_SRCS 变量
	aux_source_directory(${TOP_SRC}          DIR_SRCS)
	aux_source_directory(${TOP_SRC}/MetaData DIR_SRCS)
	aux_source_directory(${TOP_SRC}/Utils    DIR_SRCS)
	aux_source_directory(${TOP_SRC}/Dper     DIR_SRCS)
	aux_source_directory(${TOP_SRC}/DPC      DIR_SRCS)
	# 指定生成目标
	add_executable(Imager main.cpp ${DIR_SRCS})
endif()

# 添加第三方库
#link_directories("./sdk/lib")
FIND_LIBRARY(sys ./sdk/lib)
