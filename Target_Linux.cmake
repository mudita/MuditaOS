set(PROJECT_TARGET "TARGET_Linux" CACHE INTERNAL "")

set(TARGET_SOURCES CACHE INTERNAL "")

set(TARGET_DIR_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/board/linux  CACHE INTERNAL "")

set(TARGET_COMPILE_FEATURES CACHE INTERNAL "")

set(TARGET_COMPILE_DEFINITIONS CACHE INTERNAL "")


add_compile_options(-funsigned-char)

set(TARGET_LIBRARIES
    rt
    pthread
    portaudio
    CACHE INTERNAL "" )


option (LINUX_ENABLE_SANITIZER "Enable address sanitizer for Linux" ON)
if (LINUX_ENABLE_SANITIZER)
    add_compile_options(-fsanitize=address -static-libasan)
    add_link_options(-fsanitize=address -static-libasan)
endif (LINUX_ENABLE_SANITIZER)

set(CMAKE_STRIP strip CACHE INTERNAL "")
set(CMAKE_OBJCOPY objcopy CACHE INTERNAL "")
