set(BOOST_BUILD_DIR "${BOOST_DIR}/build")
set(BOOST_FILES
	"${BOOST_BUILD_DIR}/lib/libboost_thread.a"
	"${BOOST_BUILD_DIR}/lib/libboost_system.a"
	"${BOOST_BUILD_DIR}/lib/libboost_filesystem.a"
	"${BOOST_BUILD_DIR}/lib/libboost_program_options.a"
	"${BOOST_BUILD_DIR}/lib/libboost_atomic.a"
	"${BOOST_BUILD_DIR}/lib/libboost_chrono.a"
	"${BOOST_BUILD_DIR}/lib/libboost_date_time.a"
)
set(BOOST_BUILD_OPTIONS
	--prefix=${BOOST_BUILD_DIR}
	--with-libraries=thread,system,filesystem,program_options,atomic,chrono,date_time
	runtime-link=static
	link=static
)

# builds boost
function(build_boost)
	find_path(BOOST_EXISTS libboost_system.a ${BOOST_BUILD_DIR}/lib)
	message("BOOST_EXISTS ${BOOST_EXISTS}")
	if((NOT BOOST_EXISTS) OR (NOT EXISTS ${BOOST_BUILD_DIR}/lib/libboost_system.a))
		message("boost building at ${BOOST_DIR}")
		execute_process(
			COMMAND git submodule update --init
			WORKING_DIRECTORY ${BOOST_DIR}
		)
		execute_process(
			COMMAND mkdir -p ${BOOST_BUILD_DIR}
			WORKING_DIRECTORY ${BOOST_DIR}
		)
		execute_process(
			COMMAND ./bootstrap.sh ${BOOST_BUILD_OPTIONS}
			WORKING_DIRECTORY ${BOOST_DIR}
		)
		execute_process(
			COMMAND ./b2 install
			WORKING_DIRECTORY ${BOOST_DIR}
		)
		message("boost built at ${BOOST_BUILD_DIR}")
	else()
		message("boost found at ${BOOST_BUILD_DIR}")
	endif()
endfunction()

