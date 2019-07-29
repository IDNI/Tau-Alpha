cmake_minimum_required(VERSION 3.13)
project(alpha)

set(CMAKE_VERBOSE_MAKEFILE true)
set(TML_DIR ${PROJECT_SOURCE_DIR}/submodules/TML)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(alpha-common)
include(init_submodule)
init_submodule(${TML_DIR})

set(ALPHA_HEADERS
	src/Wt/alpha.h
	src/Wt/codemirror.h
	src/Wt/TML_editor.h
	src/Wt/splitjs.h
)
set(ALPHA_SOURCES
	src/main.cpp
	src/Wt/alpha.cpp
	src/Wt/runtime.cpp
)
set(ALPHA_COMPILE_OPTION
	"$<$<CONFIG:DEBUG>:-O0;-ggdb3>"
	"$<$<CONFIG:RELEASE>:-O3;-flto>"
)

find_package(Wt REQUIRED MODULE)

add_subdirectory(${TML_DIR})
set_target_properties(tml TML PROPERTIES
	EXCLUDE_FROM_ALL 1 # do not build tml executable by default
	EXCLUDE_FROM_DEFAULT_BUILD 1) # ... library is enough

add_executable(alpha)

# add tml.js if emscripten available
list(APPEND CMAKE_MODULE_PATH ${TML_DIR}/cmake)
include(emscripten_toolchain)
emscripten_toolchain_available(EMSCRIPTEN_TOOLCHAIN_AVAILABLE)
if(${EMSCRIPTEN_TOOLCHAIN_AVAILABLE})
	include(add_tml_js_lib)
	add_tml_js_lib()
	add_dependencies(alpha tml_js_lib)
else()
	message("emscripten toolchain not found. skipping tml.js")
endif()

target_setup(alpha)
target_sources(alpha PRIVATE ${ALPHA_SOURCES} ${ALPHA_HEADERS})
target_include_directories(alpha PRIVATE ${Wt_INCLUDE_DIR})
target_link_libraries(alpha TMLs ${Wt_LIBRARY} ${Wt_HTTP_LIBRARY})
