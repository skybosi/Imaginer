# Make 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息
project (Imager)

# 指定include 路径
include_directories("include")

# 查找include目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(include DIR_SRCS)
# 指定生成目标
add_executable(Imager src/bmp.cpp ${DIR_SRCS})

# 添加第三方库
#link_directories("./sdk/lib")
FIND_LIBRARY(sys ./sdk/lib)
