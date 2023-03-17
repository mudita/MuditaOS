include(ExternalProject)

function(add_icu)
    cmake_parse_arguments(
            ICU
            "CROSS"
            "DIR;INSTALL;FILTER"
            ""
            ${ARGN}
    )

    # Even if we are cross-compiling, we need to build ICU sources on a host first.
    # On macOS, gcc is linked to clang which MuditaOS does not support. 'Real' gcc can be used by specifying its version, i.e., gcc-xx.
    # The lines below try to find the most up-to-date gcc version(user may be have several versions of gcc installed). First it tries to look for a specific gcc version(this step handles macOS).
    # As a last resort, it tries to use regular 'gcc' which, on Linux happens to be a desirable gcc version.
    find_program(HOST_CC gcc-12 gcc-11 gcc-10 gcc REQUIRED)
    find_program(HOST_CXX g++-12 g++-11 g++-10 g++ REQUIRED)

    # Host build
    ExternalProject_Add(icu_sources
            SOURCE_DIR ${ICU_DIR}/src/icu4c/source
            PREFIX icu
            INSTALL_DIR install_host
            BINARY_DIR build_host
            LOG_CONFIGURE 1
            LOG_BUILD 1
            LOG_INSTALL 1
            LOG_OUTPUT_ON_FAILURE 1
            CONFIGURE_COMMAND
            ${CMAKE_COMMAND} -E
            env ICU_DATA_FILTER_FILE=${ICU_FILTER}
            env CC=${HOST_CC}
            env CXX=${HOST_CXX}
            <SOURCE_DIR>/configure
            -q
            --enable-static
            --with-data-packaging=files
            --disable-shared
            --disable-dyload
            --disable-tests
            --disable-samples
            --enable-weak-threads
            -prefix=<INSTALL_DIR>
            )

    ExternalProject_Get_property(icu_sources INSTALL_DIR)
    ExternalProject_Add_Step(icu_sources populate_data
            DEPENDEES install
            ALWAYS 1
            COMMAND mkdir -p ${ICU_INSTALL}
            COMMAND rsync -a ${INSTALL_DIR}/share/icu/72.1/ ${ICU_INSTALL}
            )

    # Cross build
    ExternalProject_Get_property(icu_sources BINARY_DIR)
    set(common_flags "-I${ICU_DIR}/retarget -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard")
    set(cxx_flags "${CMAKE_CXX_FLAGS} ${common_flags}")
    set(c_flags "${CMAKE_C_FLAGS} ${common_flags}")

    ExternalProject_Add(icu_sources_cross
            SOURCE_DIR ${ICU_DIR}/src/icu4c/source
            PREFIX icu
            INSTALL_DIR install_cross
            BINARY_DIR build_cross
            LOG_CONFIGURE 1
            LOG_BUILD 1
            LOG_INSTALL 1
            LOG_OUTPUT_ON_FAILURE 1
            CONFIGURE_COMMAND
            ${CMAKE_COMMAND} -E
            env ICU_DATA_FILTER_FILE=${ICU_FILTER}
            <SOURCE_DIR>/configure
            --host=arm-none-eabi
            --with-cross-build=${BINARY_DIR}
            -q
            --enable-static
            --enable-weak-threads
            --with-data-packaging=files
            --disable-shared
            --disable-dyload
            --disable-tests
            --disable-samples
            --disable-tools
            --disable-extras
            --disable-strict
            -prefix=<INSTALL_DIR>
            CXXFLAGS=${cxx_flags}
            CFLAGS=${c_flgs}
            LDFLAGS=${CMAKE_EXE_LINKER_FLAGS}
            )

    # Create new target that will include all the required internal ICU sub-libraries
    add_library(icu INTERFACE)
    if (${ICU_CROSS})
        add_dependencies(icu_sources_cross icu_sources)
        add_dependencies(icu icu_sources_cross)
        ExternalProject_Get_property(icu_sources_cross INSTALL_DIR)
    else ()
        add_dependencies(icu icu_sources)
        ExternalProject_Get_property(icu_sources INSTALL_DIR)
    endif ()

    # Propagate install directory
    target_include_directories(icu INTERFACE ${INSTALL_DIR}/include)
    target_link_directories(icu INTERFACE ${INSTALL_DIR}/lib)
    target_link_libraries(icu INTERFACE icui18n icuio icuuc icudata)

endfunction()

