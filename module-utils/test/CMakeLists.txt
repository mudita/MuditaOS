cmake_minimum_required(VERSION 3.12)

# Locate GTest
find_package(GTest NO_POLICY_SCOPE)

set( CMAKE_PROJECT_UNIT_TEST_UTF8 "unittest_utf8" )
set( CMAKE_PROJECT_UNIT_TEST_LOG "unittest_log" )
set( CMAKE_PROJECT_GTEST_UTF8 "gtest_utf8" )
#Log unit tests projeet
project(unittest_module-utils_log VERSION 1.0
       DESCRIPTION "Module-utils log unit tests")

add_executable( ${PROJECT_NAME}
"${CMAKE_CURRENT_LIST_DIR}/unittest_log.cpp"
"${CMAKE_CURRENT_LIST_DIR}/unittest_log2.cpp"

)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(${PROJECT_NAME} PRIVATE module-utils)

#UTF8 unit tests projeet
project(unittest_module-utils_utf8 VERSION 1.0
       DESCRIPTION "Module-utils utf8 unit tests")

add_executable( ${PROJECT_NAME}
"${CMAKE_CURRENT_LIST_DIR}/unittest_utf8.cpp"

)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(${PROJECT_NAME} PRIVATE module-utils)

# UTF8 gtest executable
if( GTest_FOUND )
	add_executable( ${CMAKE_PROJECT_GTEST_UTF8} "${CMAKE_CURRENT_LIST_DIR}/gtest_utf8.cpp")
	target_include_directories(${CMAKE_PROJECT_GTEST_UTF8} 
		PUBLIC 
			"${CMAKE_CURRENT_LIST_DIR}/.."
			"${GTEST_INCLUDE_DIRS}"
	)
	
	target_link_libraries( ${CMAKE_PROJECT_GTEST_UTF8} 
		#${LIBRARY_UTF8}
		${GTEST_LIBRARIES} pthread
		module-utils
	)
else()
	message( "${Red} No GTest ${ColourReset} - skipping build of ${CMAKE_PROJECT_GTEST_UTF8} ")
endif()

#Log unit tests projeet
project(unittest_module-utils_time VERSION 1.0
       DESCRIPTION "Module-utils time unit tests")

add_executable( ${PROJECT_NAME}
"${CMAKE_CURRENT_LIST_DIR}/unittest_time_conversion.cpp"

)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(${PROJECT_NAME} PRIVATE module-utils module-vfs)

#Utils unit tests project
project(unittest_module-utils_utils VERSION 1.0
       DESCRIPTION "Module-utils utils unit tests")
add_executable( ${PROJECT_NAME}
"${CMAKE_CURRENT_SOURCE_DIR}/unittest_utils.cpp"
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")
target_link_libraries(${PROJECT_NAME} PRIVATE module-utils)