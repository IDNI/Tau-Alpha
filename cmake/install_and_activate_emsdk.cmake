set(EMSDK_URL "https://github.com/emscripten-core/emsdk.git")
function(install_and_activate_emsdk)
	find_path(EMSDK_CONFIG .gitignore "${EMSDK_DIR}")
	if((NOT EMSDK_CONFIG) OR (NOT EXISTS ${EMSDK_CONFIG}))
		message("--- installing and activating emsdk: ${EMSDK_DIR}")
		execute_process(
			COMMAND mkdir -p "${EMSDK_DIR}"
		)
		execute_process(
			COMMAND git clone ${EMSDK_URL} ${EMSDK_DIR}
			WORKING_DIRECTORY ${EMSDK_DIR}
		)
		execute_process(
			COMMAND ./emsdk install latest
			WORKING_DIRECTORY ${EMSDK_DIR}
		)
		execute_process(
			COMMAND ./emsdk activate latest
			WORKING_DIRECTORY ${EMSDK_DIR}
		)
	else()
		message("--- emsdk found: ${EMSDK_DIR}")
	endif()
endfunction()

