set(PROJECT_TARGET "TARGET_Linux" CACHE INTERNAL "")

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL Darwin)
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)
endif()

add_compile_options(-funsigned-char)

option (LINUX_ENABLE_SANITIZER "Enable address sanitizer for Linux" ON)
if (LINUX_ENABLE_SANITIZER)
    add_compile_options($<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Linux>:-fsanitize=address>)
    add_link_options($<$<STREQUAL:${CMAKE_HOST_SYSTEM_NAME},Linux>:-fsanitize=address>)
endif (LINUX_ENABLE_SANITIZER)

set(CMAKE_STRIP strip CACHE INTERNAL "")
set(CMAKE_OBJCOPY objcopy CACHE INTERNAL "")
