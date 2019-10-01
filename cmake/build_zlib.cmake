set(ZLIB_FILE "${ZLIB_DIR}/libz.a")
set(ZLIB_VERSION "1.2.11")
set(ZLIB_URL "https://www.zlib.net/zlib-${ZLIB_VERSION}.tar.gz")
function(build_zlib)
	find_path(ZLIB_EXISTS libz.a ${ZLIB_DIR})
	if((NOT ZLIB_EXISTS) OR (NOT EXISTS ${ZLIB_FILE}))
		message("--- zlib not found. building at ${ZLIB_DIR}")
		execute_process(
			COMMAND mkdir -p "${ZLIB_DIR}"
		)
		execute_process(
			COMMAND wget -N ${ZLIB_URL}
			WORKING_DIRECTORY ${ZLIB_DIR}
		)
		execute_process(
			COMMAND tar xzf zlib-${ZLIB_VERSION}.tar.gz --strip-components=1 -C .
			WORKING_DIRECTORY ${ZLIB_DIR}
		)
		execute_process(
			COMMAND ./configure
			WORKING_DIRECTORY ${ZLIB_DIR}
		)
		execute_process(
			COMMAND make libz.a
			WORKING_DIRECTORY ${ZLIB_DIR}
		)
		message("--- zlib built ${ZLIB_FILE}")
	else()
		message("--- zlib found ${ZLIB_FILE}")
	endif()
endfunction()

