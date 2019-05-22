cmake_minimum_required(VERSION 3.12)

set( CMAKE_PROJECT_UNIT_TEST_GUI "unittest_gui" )

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set (CMAKE_CXX_STANDARD 14)

#Unit test for gui
project(${CMAKE_PROJECT_UNIT_TEST_GUI} VERSION 1.0
       DESCRIPTION "Test program for utf8 strings."
       LANGUAGES CXX)
 

       
add_executable( ${CMAKE_PROJECT_UNIT_TEST_GUI} "${CMAKE_CURRENT_LIST_DIR}/unittest_gui.cpp")
target_include_directories(${CMAKE_PROJECT_UNIT_TEST_GUI} 
	PUBLIC 
		"${CMAKE_CURRENT_LIST_DIR}/.."
		"${CMAKE_CURRENT_LIST_DIR}/../module-utils"
		"${CMAKE_CURRENT_LIST_DIR}/../gui"
)

target_link_directories(${CMAKE_PROJECT_UNIT_TEST_GUI} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../lib" )

target_link_libraries( ${CMAKE_PROJECT_UNIT_TEST_GUI} PRIVATE module-gui )

find_library(LIBRT rt) 
if(LIBRT)
	target_link_libraries(${CMAKE_PROJECT_UNIT_TEST_GUI} PRIVATE ${LIBRT})
endif()
