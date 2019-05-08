set(PROJECT_TARGET "TARGET_Linux" CACHE INTERNAL "")

set(TARGET_SOURCES


        CACHE INTERNAL "")

set(TARGET_DIR_INCLUDES

        ${CMAKE_CURRENT_LIST_DIR}/board/linux

        CACHE INTERNAL "")

set(TARGET_COMPILE_FEATURES

        CACHE INTERNAL "")

set(TARGET_COMPILE_DEFINITIONS

        CACHE INTERNAL ""
        )

set(TARGET_COMPILE_OPTIONS

        -fsanitize=address

        CACHE INTERNAL ""
        )


set(TARGET_LIBRARIES

        pthread

        CACHE INTERNAL ""
        )

set(TARGET_LINK_OPTIONS

        -fsanitize=address

        CACHE INTERNAL ""
        )