# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# function to add:
# - module library to compilation
# - module definition to compilation
# - cmake option enabled by default
# this way we can enable/disable apps or services in cmake
function(add_module_options)

    cmake_parse_arguments( _ARG
                           ""
                           "OPTION_PREFIX;CATALOG_PREFIX;DEFINES_LIST_NAME"
                           "IN_MODULE_NAMES"
                            ${ARGN}
                           )

    message("configurables added: ${_ARG_IN_MODULE_NAMES}")

    list(SORT _ARG_IN_MODULE_NAMES)
    foreach(app IN LISTS _ARG_IN_MODULE_NAMES)

        message("-> configuring option and define for: ${app}")

        string(TOUPPER ${app} app_upper)
        string(REPLACE "-" "_" app_macro_name ${app_upper})

        #add option for each element from the IN_MODULE_NAMES
        if (NOT DEFINED ENABLE_${_ARG_OPTION_PREFIX}_${app_macro_name})
            message("    adding option: ${app}")
            option(ENABLE_${_ARG_OPTION_PREFIX}_${app_macro_name} "Enable ${_ARG_CATALOG_PREFIX} ${app}" ON)
        endif()

        # add #define for each application to list
        if(ENABLE_${_ARG_OPTION_PREFIX}_${app_macro_name})
            message("    adding definition for: ${app}")
            add_subdirectory(${_ARG_CATALOG_PREFIX}-${app})
            list(APPEND _TMP "ENABLE_${_ARG_OPTION_PREFIX}_${app_macro_name}")
        endif()
    endforeach()


    set(${_ARG_DEFINES_LIST_NAME} ${_TMP} PARENT_SCOPE)
    message("defines for ${_ARG_DEFINES_LIST_NAME} set to: ${_TMP}")

endfunction()
