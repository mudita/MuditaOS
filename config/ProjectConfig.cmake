set(PROJECT_CONFIG_DEFINITIONS

        LOG_USE_COLOR=1
        PROJECT_CONFIG_USER_DYNMEM_SIZE=10*1024*1024
        MODULE_CORE_CUSTOM_BUS
        LOG_REDIRECT_TO_SERIAL=0

        CACHE INTERNAL ""
        )

    #message("Select bt!")
    #if(NOT DEFINED BT_STACK_SEL)
    #    if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    # message(STATUS "Bluetopia wont work with Linux, select BlueKitchen")
        # set(BT_STACK "BlueKitchen")
        #    else()
        #        message(STATUS "Bluetopia selected default for CrossDev")
        #        set(BT_STACK "Bluetopia")
        #    endif()
        #else()
        #    if(BT_STACK_SEL STREQUAL "BlueKitchen")
                 message(STATUS "BlueKitchen selected")
                 set(BT_STACK "BlueKitchen")
        #    else()
        #       message(STATUS "Bluetopia selected")
        #       set(BT_STACK "Bluetopia")
        #    endif()
        #endif()

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)
