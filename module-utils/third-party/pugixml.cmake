include (thirdparty)

set (PUGIXML_TARGET pugixml)

# setup pufixml path and source list
set (PUGIXML_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/pugixml/src)
set (PUGIXML_SOURCES
    ${PUGIXML_SRCDIR}/pugixml.cpp
)

# create static library for the third party
add_library (${PUGIXML_TARGET} STATIC ${PUGIXML_SOURCES})

# setup flags for the third party
third_party_target_setup (${PUGIXML_TARGET})

# add include directory path
target_include_directories (${PUGIXML_TARGET} PUBLIC ${PUGIXML_SRCDIR})

# turn on optimization in debug
third_party_source_optimization (${PUGIXML_SOURCES})
