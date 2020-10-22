include (thirdparty)

# add tinyexpr library sources
set (TINYEXPR_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/tinyexpr)
set (TINYEXPR_SOURCES
        ${TINYEXPR_SRCDIR}/tinyexpr.c
)

# create static library for the third party
set (TINYEXPR_TARGET tinyexpr)
add_library (${TINYEXPR_TARGET} STATIC ${TINYEXPR_SOURCES})

# setup flags for the third party
third_party_target_setup (${TINYEXPR_TARGET})

# add include directory path
target_include_directories (${TINYEXPR_TARGET} PUBLIC ${TINYEXPR_SRCDIR})

# optimize thirdy party sources in dbeug
third_party_source_optimization (${TINYEXPR_SOURCES})
