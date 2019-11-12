cmake_minimum_required(VERSION 3.13)
project(alpha_repl)

set(CMAKE_VERBOSE_MAKEFILE true)

set(ALPHA_DIRECTORY "${PROJECT_SOURCE_DIR}/../../..")

list(APPEND CMAKE_MODULE_PATH ${ALPHA_DIRECTORY}/cmake)

message("ALPHA_DIRECTORY ${ALPHA_DIRECTORY}")

include(alpha-common)

set(TML_DIR   ${PROJECT_SOURCE_DIR}/TML)
set(EMSDK_DIR ${ALPHA_DIRECTORY}/lib/emsdk)

include(install_and_activate_emsdk)
install_and_activate_emsdk(${EMSDK_DIR})

set(ALPHA_REPL_HEADERS
	repl.h
	../agent.h
	../channel.h
	../defs.h
	../filter.h
	../message.h
	../protocol.h
	../storage.h
)
set(ALPHA_REPL_SOURCES
	main.cpp
	repl.cpp
	../filter.cpp
	../output.cpp
	../protocol_nonet.cpp
)
set(ALPHA_REPL_COMPILE_OPTION
	"$<$<CONFIG:DEBUG>:-O0;-ggdb3>"
	"$<$<CONFIG:RELEASE>:-O3;-flto>"
)

# TML
add_subdirectory(${TML_DIR})
set_target_properties(tml TMLso PROPERTIES
	EXCLUDE_FROM_ALL 1 # do not build tml executable and so by default
	EXCLUDE_FROM_DEFAULT_BUILD 1) # ... static library is enough
add_executable(alpha-repl)

target_setup(alpha-repl)
target_sources(alpha-repl PRIVATE ${ALPHA_REPL_SOURCES} ${ALPHA_REPL_HEADERS})
target_link_libraries(alpha-repl TMLs)

