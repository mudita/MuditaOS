add_subdirectory(src)

target_compile_definitions(re2 PUBLIC RE2_USE_RTOS_WRAPPER)

target_compile_options (re2 PRIVATE -Wno-missing-field-initializers)

target_link_libraries(re2 PUBLIC module-os log)
