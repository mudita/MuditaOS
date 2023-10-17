add_subdirectory(src/cmake)
set_target_properties(libprotobuf PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
target_compile_options(libprotobuf-lite PRIVATE -w)
