cmake_minimum_required(VERSION 3.13)
project(alpha)

################################################################################
option(ENABLE_LOCAL_EVALUATION "Enable local evaluation" ON)
option(DISABLE_SERVER_EVALUATION "Forbid evaluation on the server" OFF)
################################################################################

set(CMAKE_VERBOSE_MAKEFILE true)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(alpha-common)

set(TML_DIR   ${PROJECT_SOURCE_DIR}/submodules/TML)
set(WT_DIR    ${PROJECT_SOURCE_DIR}/submodules/wt)
set(BOOST_DIR ${PROJECT_SOURCE_DIR}/submodules/boost)
set(ZLIB_DIR  ${PROJECT_SOURCE_DIR}/submodules/zlib)
set(EMSDK_DIR ${PROJECT_SOURCE_DIR}/submodules/emsdk)

include(init_submodule)
init_submodule(${TML_DIR})
init_submodule(${ZLIB_DIR})
init_submodule(${BOOST_DIR})
init_submodule(${WT_DIR})
init_submodule(${EMSDK_DIR})

include(install_and_activate_emsdk)
install_and_activate_emsdk(${EMSDK_DIR})

set(ALPHA_HEADERS
	src/Wt/alpha.h
	src/Wt/codemirror.h
	src/Wt/TML_editor.h
	src/Wt/splitjs.h
	src/Wt/workspace.h
	src/Wt/file_item.h
)
set(ALPHA_SOURCES
	src/main.cpp
	src/Wt/alpha.cpp
	src/Wt/runtime.cpp
	src/Wt/runtime_browser.cpp
	src/Wt/workspace.cpp
	src/Wt/file_item.cpp
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
include(build_boost)
build_boost()
set(BOOST_ROOT ${BOOST_BUILD_DIR})
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
set_target_properties(tml TML PROPERTIES
	EXCLUDE_FROM_ALL 1 # do not build tml executable by default
	EXCLUDE_FROM_DEFAULT_BUILD 1) # ... library is enough

add_executable(alpha)

# add tml.js if emscripten available
set(EMSCRIPTEN_DIR "${EMSDK_DIR}/fastcomp/emscripten")
list(APPEND CMAKE_MODULE_PATH ${TML_DIR}/cmake)
include(emscripten_toolchain)
emscripten_toolchain_available(EMSCRIPTEN_TOOLCHAIN_AVAILABLE)
if(ENABLE_LOCAL_EVALUATION)
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
target_include_directories(alpha PRIVATE ${BOOST_ROOT})
target_include_directories(alpha PRIVATE ${Wt_INCLUDE_DIR})
target_link_directories(alpha PRIVATE ${ZLIB_DIR})
target_link_directories(alpha PRIVATE ${BOOST_ROOT}/lib)
target_link_directories(alpha PRIVATE ${Wt_LIBRARY_DIR})
target_link_libraries(alpha
	TMLs
	${ZLIB_LIBRARY}
	${BOOST_ROOT}/lib/libboost_program_options.a
	${BOOST_ROOT}/lib/libboost_thread.a
	${BOOST_ROOT}/lib/libboost_atomic.a
	${BOOST_ROOT}/lib/libboost_system.a
	${BOOST_ROOT}/lib/libboost_filesystem.a
	${BOOST_ROOT}/lib/libboost_chrono.a
	${BOOST_ROOT}/lib/libboost_date_time.a
	${Wt_LIBRARY}
	${Wt_HTTP_LIBRARY}
	pthread
	z
	boost_program_options boost_thread boost_atomic
	boost_system boost_filesystem boost_chrono boost_date_time
	wt wthttp
)

if(NOT ENABLE_LOCAL_EVALUATION)
	target_compile_definitions(alpha PRIVATE "DISABLE_LOCAL_EVALUATION=1")
endif()
if(DISABLE_SERVER_EVALUATION)
	target_compile_definitions(alpha PRIVATE "DISABLE_SERVER_EVALUATION=1")
endif()
