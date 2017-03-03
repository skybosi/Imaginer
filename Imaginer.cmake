# set build type
set(CMAKE_BUILD_TYPE "Debug")

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-unused-variable -pthread")
else( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	set(CMAKE_BUILD_TYPE "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pthread -fopenmp")
endif( CMAKE_BUILD_TYPE STREQUAL "Debug" )

message("Build Type:" ${CMAKE_BUILD_TYPE} ${CMAKE_CXX_FLAGS})

## 指定include 路径
message("Project path:" ${PROJECT_SOURCE_DIR})

set(TOP_INCLUDE ${PROJECT_SOURCE_DIR}/include)
set(TOP_SRC ${PROJECT_SOURCE_DIR}/src)

include_directories(${TOP_INCLUDE}
	${TOP_INCLUDE}/BaseData
	${TOP_INCLUDE}/Commons
	${TOP_INCLUDE}/DPC
	${TOP_INCLUDE}/DPer
	${TOP_INCLUDE}/MetaData
	${TOP_INCLUDE}/Utils )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
