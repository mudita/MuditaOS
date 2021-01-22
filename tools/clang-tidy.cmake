find_program(RUN_CLANG_TIDY_COMMAND NAMES run-clang-tidy)
if (NOT RUN_CLANG_TIDY_COMMAND)
	message(WARNING "run-clang-tidy can not be found.")
	return()
endif()

if (NOT CMAKE_EXPORT_COMPILE_COMMANDS)
	message(WARNING "Unable to run clang-tidy without the compile commands database.")
	return()
endif()

set(STATIC_ANALYSIS_OUTPUT_DIR "StaticAnalysis")

string(TIMESTAMP CURRENT_TIME)
set(CLANG_TIDY_OUTPUL_FILE "clang-tidy_${CURRENT_TIME}")

add_custom_target(clang-tidy
	COMMAND ${CMAKE_COMMAND} -E make_directory ${STATIC_ANALYSIS_OUTPUT_DIR}
	COMMAND ${RUN_CLANG_TIDY_COMMAND} -header-filter='.*' -p ${CMAKE_BINARY_DIR} > "${STATIC_ANALYSIS_OUTPUT_DIR}/${CLANG_TIDY_OUTPUL_FILE}")
