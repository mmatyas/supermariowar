find_package(Git)
if(GIT_FOUND)
	execute_process(COMMAND
	    "${GIT_EXECUTABLE}"
	    --git-dir "${PROJECT_SOURCE_DIR}/.git"
	    --work-tree "${PROJECT_SOURCE_DIR}"
	    describe
	    --always
	    --dirty
	    OUTPUT_STRIP_TRAILING_WHITESPACE
	    OUTPUT_VARIABLE SMW_GIT_REVISION
	)
	execute_process(COMMAND
	    "${GIT_EXECUTABLE}"
	    --git-dir "${PROJECT_SOURCE_DIR}/.git"
	    --work-tree "${PROJECT_SOURCE_DIR}"
	    log -1
	    --format=%cd
	    --date=short
	    OUTPUT_STRIP_TRAILING_WHITESPACE
	    OUTPUT_VARIABLE SMW_GIT_DATE
	)
endif()
