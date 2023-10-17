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

add_catch2_executable(
    NAME
        cpu-algorithms
    SRCS
        test-cpu-algorithms.cpp
    LIBS
        module-sys
)

add_catch2_executable(
    NAME
        task-statistics
    SRCS
        test-taskStatistics.cpp
    LIBS
        module-sys
)
