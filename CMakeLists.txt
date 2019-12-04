CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(redisgraph)

SET(CMAKE_CXX_COMPILER "/usr/local/Cellar/gcc/8.3.0/bin/g++-8")
SET(CMAKE_C_COMPILER "/usr/local/Cellar/gcc/8.3.0/bin/gcc-8")

SET(CMAKE_C_STANDARD 99)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -g -ggdb -std=c99 -O2 -Wunused-parameter")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmacosx-version-min=10.12")
IF (APPLE)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -undefined dynamic_lookup")
ENDIF()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_MACOSX_RPATH 1)
endif()

ADD_SUBDIRECTORY(src)