set (CMAKE_CXX_STANDARD 17)

project( service_renderer VERSION 1.0
	DESCRIPTION "GTK application for showing draw buffer."
	LANGUAGES CXX )

find_package(PkgConfig REQUIRED)               
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

add_executable( ${PROJECT_NAME} 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui_renderer.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RArea.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RArea.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RWindow.hpp )

target_link_libraries( ${PROJECT_NAME}  ${GTKMM_LIBRARIES}  )
target_include_directories(${PROJECT_NAME} PUBLIC ${GTKMM_LIBRARY_DIRS} )
target_include_directories( ${PROJECT_NAME}  PUBLIC ${GTKMM_INCLUDE_DIRS}  )
target_link_libraries( ${PROJECT_NAME} ${LIBRT} rt pthread )
   
#key_code
target_include_directories( ${PROJECT_NAME}  PUBLIC "${CMAKE_SOURCE_DIR}/"  )
target_compile_options(${PROJECT_NAME} PUBLIC "-Wno-deprecated-declarations")
target_compile_options(${PROJECT_NAME} PUBLIC "-Wno-unused-result")
message(WARNING "Please restore -Wno-error and -Wall flags with docker with gtk3 >= 3.24")
target_compile_options(${PROJECT_NAME} PRIVATE -Wno-error -Wno-all)
