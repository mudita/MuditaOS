add_library(utils-phonenumber STATIC)

target_sources(utils-phonenumber
    PUBLIC
        NumberHolderMatcher.hpp
        PhoneNumber.hpp

    PRIVATE
        PhoneNumber.cpp
)

target_include_directories(utils-phonenumber
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

target_link_libraries(utils-phonenumber
    PUBLIC
        utf8
        utils-locale
        libphonenumber::libphonenumber
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif()
