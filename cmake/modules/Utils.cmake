# An equivalent of install() which allows to declare multiple components using
# a custom 'COMPONENTS' clause. This clause must be the last on the
# argument list. The original 'COMPONENT' from install() clause must not appear
# on the argument list.  
function(multicomp_install)
    list(FIND ARGN "COMPONENTS" CLAUSE_INDEX)
    list(SUBLIST ARGN 0 ${CLAUSE_INDEX} INSTALL_ARGN)
    math(EXPR COMPS_INDEX "${CLAUSE_INDEX}+1")
    list(SUBLIST ARGN ${COMPS_INDEX} ${ARGC} COMPONENTS)
    foreach(COMP IN LISTS COMPONENTS)
        install(${INSTALL_ARGN} COMPONENT ${COMP})
    endforeach()
endfunction()

macro(print_var VARIABLE)
    if(${VARIABLE})
        message(STATUS "${Green}${VARIABLE}: '${Orange}${${VARIABLE}}${ColourReset}'")
    else()
        message(STATUS "${Orange}No such variable: '${Red}${VARIABLE}${ColourReset}'")
    endif()
endmacro()
