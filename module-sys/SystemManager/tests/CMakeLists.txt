add_catch2_executable(
    NAME
        dependency-graph-tests
    SRCS
        test-DependencyGraph.cpp
    LIBS
        module-sys
)

add_catch2_executable(
    NAME
        PowerManager
    SRCS
        unittest_CpuSentinelsGovernor.cpp
    LIBS
        module-sys
)
