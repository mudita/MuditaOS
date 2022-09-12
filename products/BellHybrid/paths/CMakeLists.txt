add_library(paths Paths.cpp)
add_library(bell::paths ALIAS paths)

target_include_directories(paths PUBLIC .)

target_link_libraries(paths PRIVATE module-vfs)