add_library(board INTERFACE)
add_subdirectory(libiosyscalls)
add_subdirectory(crashdump-serial-number)
target_compile_definitions(board-config INTERFACE PROJECT_CONFIG_USER_DYNMEM_SIZE=0)
