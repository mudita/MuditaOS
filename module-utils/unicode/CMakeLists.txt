add_library(utils-unicode INTERFACE)

add_subdirectory(ucs2)
add_subdirectory(utf8)

target_link_libraries(utils-unicode
    INTERFACE
        ucs2
        utf8
)
