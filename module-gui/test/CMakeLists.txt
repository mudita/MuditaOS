project("unittest_gui" VERSION 1.0 DESCRIPTION "GUI tests.")

add_executable(${PROJECT_NAME} EXCLUDE_FROM_ALL
    test-gui.cpp
    test-context.cpp
    test-gui-callbacks.cpp
    test-gui-resizes.cpp
    mock/TestWindow.cpp
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_SOURCE_DIR}/module-utils"
    "${CMAKE_SOURCE_DIR}/module-vfs"
)

target_link_directories(${PROJECT_NAME} PUBLIC "${PROJECT_LIB_DIRECTORY}")
target_link_libraries(${PROJECT_NAME} PUBLIC module-vfs module-utils module-gui)

add_test(NAME ${PROJECT_NAME}
         COMMAND ${PROJECT_NAME}
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
         )

add_dependencies(check ${PROJECT_NAME})
