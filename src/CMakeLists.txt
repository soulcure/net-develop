cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.
project("net-server") 

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

# header file dir
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/net)
include_directories(${PROJECT_SOURCE_DIR}/net/tcp/client)
include_directories(${PROJECT_SOURCE_DIR}/net/tcp/server)
include_directories(${PROJECT_SOURCE_DIR}/tools)

# source file dir
aux_source_directory(. DIR_SRCS)
aux_source_directory(./net NET)
aux_source_directory(./net/tcp/client NET_CLIENT)
aux_source_directory(./net/tcp/server NET_SERVER)
aux_source_directory(./tools TOOLS)

# gen executable file        
add_executable(${PROJECT_NAME} ${DIR_SRCS} ${NET} ${NET_CLIENT} ${NET_SERVER} ${TOOLS})



link_directories("/usr/local/lib/")

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.
target_link_libraries(${PROJECT_NAME} protobuf -lpthread)








        