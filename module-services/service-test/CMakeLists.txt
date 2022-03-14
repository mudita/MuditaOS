project(service-test)
message("${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )


add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} PRIVATE ServiceTest.cpp)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    module-sys
    PUBLIC
)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif ()
