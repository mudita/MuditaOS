add_library(crashdump-metadata-store STATIC)

target_include_directories(crashdump-metadata-store
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
        )

target_sources(crashdump-metadata-store
        PUBLIC
        CrashdumpMetadataStore.cpp
        )

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
