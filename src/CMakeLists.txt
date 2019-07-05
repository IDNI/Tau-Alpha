cmake_minimum_required(VERSION 3.1)
project(alpha)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE true)
set(SUBMODULES_DIR "${PROJECT_SOURCE_DIR}/../submodules")
set(TML_DIR "${SUBMODULES_DIR}/TML/src")

add_executable(alpha
	main.cpp
	Wt/alpha.h
	Wt/alpha.cpp
	Wt/splitjs.h
	Wt/codemirror.h
	Wt/TML_editor.h
)
include_directories(${TML_DIR})
find_library(TML_LIB NAMES TML PATHS ${TML_DIR})
target_link_libraries(alpha wthttp wt ${TML_LIB})

set(CMAKE_CXX_FLAGS "-W -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -ggdb3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()
