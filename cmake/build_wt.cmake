set(WT_BUILD_DIR "${WT_DIR}/build")
set(WT_INSTALL_DIR "${WT_DIR}/install")
set(WT_FILES
	"${WT_INSTALL_DIR}/lib/libwt.a"
	"${WT_INSTALL_DIR}/lib/libwthttp.a"
)

set(WT_BUILD_OPTIONS
	-DCMAKE_BUILD_TYPE=Release
	-DCONNECTOR_HTTP=ON
	-DCONFIGDIR=${WT_INSTALL_DIR}
	-DCMAKE_INSTALL_PREFIX=${WT_INSTALL_DIR}
	-DBoost_NO_BOOST_CMAKE=TRUE
	-DBoost_NO_SYSTEM_PATHS=TRUE
	-DBoost_USE_STATIC_LIBS=ON
	-DBoost_USE_STATIC_RUNTIME=ON
	-DBOOST_INCLUDEDIR=${Boost_INCLUDE_DIRS}
	-DBOOST_LIBRARYDIR=${Boost_LIBRARY_DIRS}
	-DZLIB_LIBRARY=${ZLIB_LIBRARY}
	-DZLIB_INCLUDE_DIR=${ZLIB_INCLUDE_DIR}
	-DBUILD_EXAMPLES=OFF
	-DENABLE_SSL=OFF
	-DENABLE_HARU=OFF
	-DENABLE_PANGO=OFF
	-DENABLE_SQLITE=OFF
	-DENABLE_POSTGRES=OFF
	-DENABLE_FIREBIRD=OFF
	-DENABLE_MYSQL=OFF
	-DENABLE_MSSQLSERVER=OFF
	-DENABLE_QT4=OFF
	-DENABLE_QT5=OFF
	-DENABLE_LIBWTTEST=OFF
	-DENABLE_LIBWTDBO=OFF
	-DCONNECTOR_FCGI=OFF
	-DBUILD_TESTS=OFF
	-DINSTALL_RESOURCES=OFF
	-DSHARED_LIBS=OFF
)
set(WT_VERSION "4.1.1")
set(WT_URL "https://github.com/emweb/wt/archive/${WT_VERSION}.tar.gz")

# builds wt
function(build_wt)
	find_path(WT_EXISTS libwt.a ${WT_INSTALL_DIR}/lib)
	if((NOT WT_EXISTS) OR (NOT EXISTS ${WT_INSTALL_DIR}/lib/libwt.a))
		message("--- wt building at ${WT_BUILD_DIR}")
		execute_process(
			COMMAND mkdir -p "${WT_DIR}"
		)
		execute_process(
			COMMAND wget -N ${WT_URL}
			WORKING_DIRECTORY ${WT_DIR}
		)
		execute_process(
			COMMAND tar xzf ${WT_VERSION}.tar.gz --strip-components=1 -C .
			WORKING_DIRECTORY ${WT_DIR}
		)
		execute_process(
			COMMAND mkdir -p ${WT_BUILD_DIR} ${WT_INSTALL_DIR}
			WORKING_DIRECTORY ${WT_DIR}
		)
		execute_process(
			COMMAND cmake --output-debug .. ${WT_BUILD_OPTIONS}
			WORKING_DIRECTORY ${WT_BUILD_DIR}
		)
		execute_process(
			COMMAND make wt wthttp
			WORKING_DIRECTORY ${WT_BUILD_DIR}
		)
		execute_process(
			COMMAND make install
			WORKING_DIRECTORY ${WT_BUILD_DIR}
		)
		message("--- wt built at ${WT_BUILD_DIR}")
	else()
		message("--- wt found ${WT_DIR}")
	endif()
	set(CMAKE_MODULE_PATH "${WT_INSTALL_DIR}/lib/cmake/wt" ${CMAKE_MODULE_PATH} PARENT_SCOPE)
endfunction()
