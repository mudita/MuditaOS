project(googletest-gui)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(${PROJECT_NAME} EXCLUDE_FROM_ALL
        test-gui-listview.cpp
        ../mock/TestWindow.cpp
        ../mock/TestListViewProvider.cpp
        )


target_include_directories(${PROJECT_NAME}
        PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_SOURCE_DIR}/module-utils"
        "${CMAKE_SOURCE_DIR}/module-vfs"
        )

target_link_directories(${PROJECT_NAME} PUBLIC "${PROJECT_LIB_DIRECTORY}")

target_link_libraries(${PROJECT_NAME} PUBLIC module-vfs module-utils module-gui)

target_link_libraries(${PROJECT_NAME} PUBLIC gtest gtest_main)

add_dependencies(check ${PROJECT_NAME})
