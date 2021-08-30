add_catch2_executable(
        NAME
        cellular-request-factory
        SRCS
        unittest_request_factory.cpp
        LIBS
        module-sys
        module-cellular
)

add_catch2_executable(
        NAME
        cellular-simcard
        SRCS
        unittest_simcard.cpp
        LIBS
        module-cellular
        module-sys
)

add_catch2_executable(
        NAME
        cellular-smssendhandler
        SRCS
        unittest_smssendhandler.cpp
        LIBS
        module-cellular
)

add_catch2_executable(
        NAME
        cellular-smsPartsHandler
        SRCS
        unittest_smsPartsHandler.cpp
        LIBS
        module-cellular
        ucs2
)

add_catch2_executable(
        NAME
        cellular-datatransfer
        SRCS
        unittest_datatransfer.cpp
        LIBS
        module-cellular
        module-sys
)

add_catch2_executable(
        NAME
        cellular-qmbn
        SRCS
        unittest_qmbn.cpp
        LIBS
        module-sys
        module-cellular
)

add_gtest_executable(
        NAME
        connection-manager
        SRCS
        unittest_connection-manager.cpp
        LIBS
        module-sys
        module-cellular
)
