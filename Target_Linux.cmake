set(PROJECT_TARGET "TARGET_Linux" CACHE INTERNAL "")

add_compile_options(-funsigned-char)

set(TARGET_LIBRARIES
    rt
    pthread
    CACHE INTERNAL "" )

link_libraries(pthread rt)

option (LINUX_ENABLE_SANITIZER "Enable address sanitizer for Linux" ON)
if (LINUX_ENABLE_SANITIZER)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif (LINUX_ENABLE_SANITIZER)

set(CMAKE_STRIP strip CACHE INTERNAL "")
set(CMAKE_OBJCOPY objcopy CACHE INTERNAL "")
