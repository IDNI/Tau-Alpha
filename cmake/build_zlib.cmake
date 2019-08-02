set(ZLIB_FILE "${ZLIB_DIR}/libz.a")
message("ZLIB_FILE ${ZLIB_FILE}")

# builds zlib
function(build_zlib)
	find_path(ZLIB_EXISTS libz.a ${ZLIB_DIR})
	if((NOT ZLIB_EXISTS) OR (NOT EXISTS ${ZLIB_FILE}))
		message("zlib not found. building at ${ZLIB_DIR}")
		execute_process(
			COMMAND ./configure
			WORKING_DIRECTORY ${ZLIB_DIR}
		)
		execute_process(
			COMMAND make libz.a
			WORKING_DIRECTORY ${ZLIB_DIR}
		)
		message("zlib built ${ZLIB_FILE}")
	else()
		message("zlib found ${ZLIB_FILE}")
	endif()
endfunction()

