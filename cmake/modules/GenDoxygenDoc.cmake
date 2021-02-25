# check if Doxygen is installed
find_package (Doxygen COMPONENTS dot)
if (DOXYGEN_FOUND)
        # set custom command scope
        unset (_DOC_ALL)
        if (BUILD_DOC_WITH_ALL)
                set (_DOC_ALL ALL)
        endif ()

        # set input and output files
        set (DOXYGEN_IN ${PROJECT_SOURCE_DIR}/doc/Doxyfile.in)
        set (DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file (${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        # add target for doxygen documentation
        add_custom_target(doc ${_DOC_ALL}
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM
        )
else ()
        if (BUILD_DOC_WITH_ALL)
                message (SEND_ERROR ${BoldRed} "Could not find doxygen with dot to build code documentation." ${ColourReset})
        else()
                message (WARNING ${BoldYellow} "Install doxygen and graphviz to be able to build code documentation" ${ColourReset})
        endif()
endif ()


