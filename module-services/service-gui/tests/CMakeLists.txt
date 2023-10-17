add_catch2_executable(
    NAME
        context-pool-tests
    SRCS
        test-ContextPool.cpp
        MockedSynchronizationMechanism.hpp
    LIBS
        service-gui
)

add_catch2_executable(
    NAME
        commands-queue-tests
    SRCS
        test-DrawCommandsQueue.cpp
        MockedSynchronizationMechanism.hpp
    LIBS
        service-gui
)

add_catch2_executable(
    NAME
        render-cache-tests
    SRCS
        test-RenderCache.cpp
    LIBS
        service-gui
)
