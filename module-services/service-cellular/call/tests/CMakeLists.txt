add_catch2_executable(
        NAME
        call-machine
        SRCS
        test-CallMachine.cpp

        LIBS
        module-cellular
        service-cellular-call
        sml::sml
        sml::utils::logger
        test::fakeit
)

set_source_files_properties(
        test-CallMachine.cpp
        PROPERTIES COMPILE_FLAGS
        "-Wno-error=unused-but-set-variable"
)
