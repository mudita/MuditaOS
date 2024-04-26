add_library(utils-language STATIC)

target_include_directories(utils-language
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

target_sources(utils-language
        PRIVATE
        LanguageUtils.cpp
        PUBLIC
        LanguageUtils.hpp
)

target_link_libraries(utils-language
        PRIVATE
        utils::i18n
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
