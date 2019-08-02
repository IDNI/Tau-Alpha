function(init_submodule submodule)
	find_path(SUBMODULE_LISTS .gitignore ${submodule})
	if((NOT SUBMODULE_LISTS) OR (NOT EXISTS ${SUBMODULE_LISTS}))
		message("Running git submodule update --init in ${submodule}")
		execute_process(COMMAND git submodule update --init --recursive
			WORKING_DIRECTORY ${submodule})
	endif()
endfunction()

