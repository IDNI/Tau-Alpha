cmake_minimum_required(VERSION 3.13)
project(alpha)

################################################################################
option(ENABLE_FRONTEND_EXECUTION "Enable executing of TML in a browser." ON)
option(DISABLE_BACKEND_EXECUTION "Disable executing of TML remotely over the ba\
ckend" OFF)
################################################################################

set(CMAKE_VERBOSE_MAKEFILE true)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(alpha-common)

set(TML_DIR   ${PROJECT_SOURCE_DIR}/submodules/TML)
set(WT_DIR    ${PROJECT_SOURCE_DIR}/lib/wt)
set(ZLIB_DIR  ${PROJECT_SOURCE_DIR}/lib/zlib)
set(EMSDK_DIR ${PROJECT_SOURCE_DIR}/lib/emsdk)

include(init_submodule)
init_submodule(${TML_DIR})

include(install_and_activate_emsdk)
install_and_activate_emsdk(${EMSDK_DIR})

set(ALPHA_HEADERS
	src/alpha/agent.h
	src/alpha/channel.h
	src/alpha/defs.h
	src/alpha/filter.h
	src/alpha/message.h
	src/alpha/protocol.h
	src/alpha/storage.h
	src/alpha/wt/app.h
	src/alpha/wt/codemirror.h
	src/alpha/wt/file_item.h
	src/alpha/wt/splitjs.h
	src/alpha/wt/TML_editor.h
	src/alpha/wt/workspace.h
	src/alpha/wt/ide/ide.h
	src/alpha/wt/ide/menu.h
	src/alpha/wt/ide/toolbar.h
	src/alpha/wt/view/agent.h
	src/alpha/wt/view/channel.h
	src/alpha/wt/view/message.h
)
set(ALPHA_SOURCES
	src/main.cpp
	src/alpha/filter.cpp
	src/alpha/output.cpp
	src/alpha/protocol_nonet.cpp
	src/alpha/storage.cpp
	src/alpha/wt/app.cpp
	src/alpha/wt/file_item.cpp
	src/alpha/wt/workspace.cpp
	src/alpha/wt/ide/ide.cpp
	src/alpha/wt/ide/runtime_backend.cpp
	src/alpha/wt/ide/runtime_frontend.cpp
	src/alpha/wt/view/agent.cpp
	src/alpha/wt/view/channel.cpp
	src/alpha/wt/view/message.cpp
)
set(ALPHA_COMPILE_OPTION
	"$<$<CONFIG:DEBUG>:-O0;-ggdb3>"
	"$<$<CONFIG:RELEASE>:-O3;-flto>"
)

# zlib
include(build_zlib)
build_zlib()
set(ZLIB_INCLUDE_DIR ${ZLIB_DIR})
find_library(ZLIB_LIBRARY NAMES z PATHS ${ZLIB_DIR})

# boost
find_package(Boost REQUIRED COMPONENTS
	"program_options;thread;atomic;system;filesystem;chrono;date_time"
)

# wt
include(build_wt)
build_wt()
find_package(Wt REQUIRED PATHS ${WT_INSTALL_DIR}/lib/cmake/wt)
set(Wt_INCLUDE_DIR ${WT_INSTALL_DIR}/include)
set(Wt_LIBRARY_DIR ${WT_INSTALL_DIR}/lib)
set(Wt_LIBRARY ${Wt_LIBRARY_DIR}/libwt.a)
set(Wt_HTTP_LIBRARY ${Wt_LIBRARY_DIR}/libwthttp.a)

# TML
add_subdirectory(${TML_DIR})
set_target_properties(tml TMLso PROPERTIES
	EXCLUDE_FROM_ALL 1 # do not build tml executable and so by default
	EXCLUDE_FROM_DEFAULT_BUILD 1) # ... static library is enough

add_executable(alpha)

# add tml.js if emscripten available
set(EMSCRIPTEN_DIR "${EMSDK_DIR}/upstream/emscripten" CACHE STRING "")
list(APPEND CMAKE_MODULE_PATH ${TML_DIR}/cmake)
include(emscripten_toolchain)
emscripten_toolchain_available(EMSCRIPTEN_TOOLCHAIN_AVAILABLE)
if(ENABLE_FRONTEND_EXECUTION)
	if(${EMSCRIPTEN_TOOLCHAIN_AVAILABLE})
		include(add_tml_js_lib)
		add_tml_js_lib()
		add_dependencies(alpha tml_js_lib)
	else()
		message("--- emscripten toolchain not found. skipping tml.js")
	endif()
endif()

target_setup(alpha)
target_sources(alpha PRIVATE ${ALPHA_SOURCES} ${ALPHA_HEADERS})
target_include_directories(alpha PRIVATE ${ZLIB_INCLUDE_DIR})
target_include_directories(alpha PRIVATE ${Wt_INCLUDE_DIR})
target_link_directories(alpha PRIVATE ${ZLIB_DIR})
target_link_directories(alpha PRIVATE ${Wt_LIBRARY_DIR})
target_link_libraries(alpha
	TMLs
	${ZLIB_LIBRARY}
	boost_program_options
	boost_thread
	boost_atomic
	boost_system
	boost_filesystem
	boost_chrono
	boost_date_time
	${Wt_LIBRARY}
	${Wt_HTTP_LIBRARY}
	pthread
	z
	wt wthttp
)

if(NOT ENABLE_FRONTEND_EXECUTION)
	target_compile_definitions(alpha PRIVATE "DISABLE_FRONTEND_EXECUTION=1")
endif()
if(DISABLE_BACKEND_EXECUTION)
	target_compile_definitions(alpha PRIVATE "DISABLE_BACKEND_EXECUTION=1")
endif()
