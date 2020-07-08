# add sources
set(SBINI ${CMAKE_CURRENT_SOURCE_DIR}/sbini)
set(SBINI_SOURCES
        ${SBINI}/sbini.cpp
)
target_sources(${PROJECT_NAME} PRIVATE ${SBINI_SOURCES})

# set compile definitions for third party libraries
target_compile_definitions(${PROJECT_NAME} PUBLIC GOOGLE_SBINI_NO_THREADS)

# supress warning for SBINI
set_source_files_properties(${SBINI_SOURCES}
        PROPERTIES COMPILE_FLAGS
	"-Wno-stringop-truncation"
)

# add include dir
target_include_directories(${PROJECT_NAME} PUBLIC ${SBINI})
