target_sources(
        module-os
    PRIVATE
        _exit.c
        fsl_tickless_gpt.c
        fsl_tickless_systick.c
        fsl_runtimestat_gpt.c

        $<$<BOOL:${SYSTEM_VIEW_ENABLED}>:
            systemview/port.c
            systemview/tasks.c
        >
        $<$<NOT:$<BOOL:${SYSTEM_VIEW_ENABLED}>>:
            port.c
        >
)

target_include_directories(
        module-os
    PUBLIC
        $<BUILD_INTERFACE:
            $<$<BOOL:${SYSTEM_VIEW_ENABLED}>:
                systemview
            >
            $<$<NOT:$<BOOL:${SYSTEM_VIEW_ENABLED}>>:
                ${CMAKE_CURRENT_SOURCE_DIR}
            >
            ${CMAKE_CURRENT_SOURCE_DIR}/include
        >
)

target_link_libraries(module-os
    PUBLIC
        fsl
        cmsis
)
