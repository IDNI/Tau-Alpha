function(install_and_activate_emsdk)
	find_path(EMSDK_CONFIG emscripten_config_fastcomp "${EMSDK_DIR}/fastcomp")
	if((NOT EMSDK_CONFIG) OR (NOT EXISTS ${EMSDK_CONFIG}))
		message("--- installing and activating emsdk: ${EMSDK_DIR}")
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

