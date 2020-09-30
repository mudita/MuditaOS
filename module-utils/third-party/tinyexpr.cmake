include(thirdparty)

# add tinyexpr library sources
set(TINYEXPR_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/tinyexpr)
set(TINYEXPR_SOURCES
        ${TINYEXPR_SRCDIR}/tinyexpr.c
        )
target_sources(${PROJECT_NAME} PRIVATE ${TINYEXPR_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${TINYEXPR_SRCDIR})
third_party_source_optimization(${TINYEXPR_SOURCES})
