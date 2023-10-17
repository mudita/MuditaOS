if (NOT CMAKE_EXPORT_COMPILE_COMMANDS)
	message(WARNING "Unable to run clang-tidy without the compile commands database.")
	return()
endif()

set(STATIC_ANALYSIS_OUTPUT_DIR "StaticAnalysis")

find_program(RUN_CLANG_TIDY_COMMAND NAMES run-clang-tidy)
if (NOT RUN_CLANG_TIDY_COMMAND)
	message(WARNING "run-clang-tidy can not be found.")
	return()
endif()

string(TIMESTAMP CURRENT_TIME)
set(CLANG_TIDY_OUTPUL_FILE "clang-tidy_${CURRENT_TIME}")

add_custom_target(clang-tidy
	COMMAND ${CMAKE_COMMAND} -E make_directory ${STATIC_ANALYSIS_OUTPUT_DIR}
	COMMAND ${RUN_CLANG_TIDY_COMMAND} -header-filter='.*' -p ${CMAKE_BINARY_DIR} > "${STATIC_ANALYSIS_OUTPUT_DIR}/${CLANG_TIDY_OUTPUL_FILE}")

find_program(GIT_COMMAND NAMES git)
if (NOT GIT_COMMAND)
	message(WARNING "git can not be found.")
	return()
endif()

find_program(CLANG_TIDY_DIFF_COMMAND NAMES clang-tidy-diff)
if (NOT CLANG_TIDY_DIFF_COMMAND)
	message(WARNING "clang-tidy-diff can not be found.")
	return()
endif()

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
	set(CLANG_TIDY_DIFF_OUTPUT_FILE "clang-tidy-diff_${CURRENT_TIME}")
	add_custom_target(clang-tidy-diff-head
		COMMAND ${CMAKE_COMMAND} -E make_directory ${STATIC_ANALYSIS_OUTPUT_DIR}
		COMMAND bash -c "cd ${CMAKE_SOURCE_DIR} && ${GIT_COMMAND} diff -U0 HEAD | ${CLANG_TIDY_DIFF_COMMAND} -p 1 -path ${CMAKE_BINARY_DIR} > \"${CMAKE_BINARY_DIR}/${STATIC_ANALYSIS_OUTPUT_DIR}/${CLANG_TIDY_DIFF_OUTPUT_FILE}\""
		VERBATIM)
else()
	message(WARNING "clang-tidy-diff target is implemented only for Unix build platform.")
endif()
