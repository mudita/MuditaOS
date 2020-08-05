find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    message("ccache found ${CCACHE_FOUND}")
    set(CMAKE_C_COMPILER_LAUNCHER   "${CCACHE_FOUND}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_FOUND}")
endif(CCACHE_FOUND)

# To show how much time we spend compiling - with ccache it should take close less then 1s per file
if(COMPILATION_TIMINGS)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")
endif(COMPILATION_TIMINGS)
