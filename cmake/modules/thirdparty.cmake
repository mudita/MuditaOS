option (THIRD_PARTY_DEBUG_OPTIMIZE "Optimize third party in debug" ON)

# optimize third party sources in debug by setting source file properties
macro (third_party_source_optimization)
    if (${THIRD_PARTY_DEBUG_OPTIMIZE} AND (${CMAKE_BUILD_TYPE} STREQUAL "Debug") AND NOT (${PROJECT_TARGET} STREQUAL "TARGET_Linux"))
        set_property(SOURCE ${ARGV} APPEND_STRING PROPERTY COMPILE_FLAGS " -Os")
    endif ()
endmacro ()

# setup flags for a third party target
macro (third_party_target_setup TARGET_NAME)
    target_compile_definitions(${TARGET_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
    target_compile_definitions(${TARGET_NAME} PUBLIC ${PROJECT_TARGET})
    target_include_directories(${TARGET_NAME} PUBLIC ${PROJECT_INCLUDES})
endmacro ()
