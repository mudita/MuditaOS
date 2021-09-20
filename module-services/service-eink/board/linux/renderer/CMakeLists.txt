set (CMAKE_CXX_STANDARD 17)

project( service_renderer VERSION 1.0
	DESCRIPTION "GTK application for showing draw buffer."
	LANGUAGES CXX )

include(Utils)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

add_executable( ${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui_renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RArea.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RArea.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RWindow.hpp )

install(TARGETS ${PROJECT_NAME} DESTINATION "./" COMPONENT Standalone)

target_link_libraries( ${PROJECT_NAME}  ${GTKMM_LIBRARIES}  )
target_include_directories(${PROJECT_NAME} PUBLIC ${GTKMM_LIBRARY_DIRS} )
target_include_directories( ${PROJECT_NAME}  PUBLIC ${GTKMM_INCLUDE_DIRS}  )
target_link_libraries( ${PROJECT_NAME} module-bsp )

#key_code
target_include_directories( ${PROJECT_NAME}  PUBLIC "${CMAKE_SOURCE_DIR}/"  )
target_compile_options(${PROJECT_NAME} PUBLIC "-Wno-deprecated-declarations")
target_compile_options(${PROJECT_NAME} PUBLIC "-Wno-unused-result")
target_compile_options(${PROJECT_NAME} PUBLIC "-Wno-parentheses")
if (NOT (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang"))
target_compile_options(${PROJECT_NAME} PUBLIC "-Wno-cast-function-type")
endif()

# disable sanitizier for target, due to bug in sigc
# https://github.com/libsigcplusplus/libsigcplusplus/issues/10
get_target_property(_target_cxx_flags ${PROJECT_NAME} COMPILE_OPTIONS)
if(_target_cxx_flags)
	list(REMOVE_ITEM _target_cxx_flags -fsanitize=address)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_OPTIONS "${_target_cxx_flags}")
endif()
get_target_property(_target_link_flags ${PROJECT_NAME} LINK_OPTIONS)
if(_target_link_flags)
	list(REMOVE_ITEM _target_link_flags -fsanitize=address)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_OPTIONS "${_target_link_flags}")
endif()

