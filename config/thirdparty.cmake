option (THIRD_PARTY_DEBUG_OPTIMIZE "Optimize third party in debug" ON)

# optimize third party sources in debug by setting source file properties
macro(third_party_source_optimization)
    if (${THIRD_PARTY_DEBUG_OPTIMIZE} AND (${CMAKE_BUILD_TYPE} STREQUAL "Debug"))
        set_property(SOURCE ${ARGV} APPEND_STRING PROPERTY COMPILE_FLAGS " -Os")
    endif()
endmacro()
