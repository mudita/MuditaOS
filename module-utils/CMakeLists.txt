﻿module_is_test_entity()

add_library(module-utils INTERFACE)

add_subdirectory(Clipboard)
add_subdirectory(CrashdumpMetadataStore)
add_subdirectory(EventStore)
add_subdirectory(i18n)
add_subdirectory(language)
add_subdirectory(locale)
add_subdirectory(log)
add_subdirectory(math)
add_subdirectory(phonenumber)
add_subdirectory(rotator)
add_subdirectory(rrule)
add_subdirectory(tar)
add_subdirectory(time)
add_subdirectory(unicode)
add_subdirectory(utility)

target_link_libraries(module-utils
    INTERFACE
        clipboard
        i18n
        log-api
        rrule
        utility
        utils-locale
        utils-math
        utils-phonenumber
        utils-time
        utils-unicode
)
