project(googletest-gui)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})

add_executable(${PROJECT_NAME} EXCLUDE_FROM_ALL
        test-gui-listview.cpp
        test-gui-boxlayout.cpp
        test-gui-gridlayout.cpp
        test-gui-depthfirst-itemtree.cpp
        test-gui-visitor-call.cpp
        test-gui-dom-dump.cpp
        ../mock/TestListViewProvider.cpp
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
        "${CMAKE_SOURCE_DIR}/module-utils"
        "${CMAKE_SOURCE_DIR}/module-gui/test/"
        )

target_link_directories(${PROJECT_NAME} PUBLIC "${PROJECT_LIB_DIRECTORY}")

target_link_libraries(${PROJECT_NAME} PUBLIC module-utils module-gui)

target_link_libraries(${PROJECT_NAME} PUBLIC gtest gtest_main)
target_link_libraries(${PROJECT_NAME} PUBLIC gmock gmock_main)

add_test(NAME ${PROJECT_NAME}
        COMMAND ${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )

add_dependencies(check ${PROJECT_NAME})
