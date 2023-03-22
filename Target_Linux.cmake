set(PROJECT_TARGET "TARGET_Linux" CACHE INTERNAL "")

add_compile_options(-funsigned-char)

set(TARGET_LIBRARIES
    rt
    pthread
    CACHE INTERNAL "" )

link_libraries(pthread rt)

set(CMAKE_STRIP strip CACHE INTERNAL "")
set(CMAKE_OBJCOPY objcopy CACHE INTERNAL "")
