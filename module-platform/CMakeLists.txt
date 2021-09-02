add_library(platform STATIC)

module_is_test_entity()

target_include_directories(
        platform

    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(
        platform

    PUBLIC
        include/Platform.hpp
)

target_link_libraries(
        platform

    PRIVATE
        module-bsp
)

add_subdirectory(${PROJECT_TARGET_NAME})
