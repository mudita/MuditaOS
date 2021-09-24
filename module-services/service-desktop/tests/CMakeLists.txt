file(
    COPY
        muditaos-unittest.tar
    DESTINATION
        ${TEST_ASSETS_DEST_DIR}/updates
)

file(
    COPY
        factory-test
    DESTINATION
        ${TEST_ASSETS_DEST_DIR}
)

add_catch2_executable(
    NAME
        service-desktop
    SRCS
        unittest.cpp
        tests-main.cpp
    LIBS
        service-desktop
        module-utils
        module-apps
    USE_FS
)

target_include_directories(
        catch2-service-desktop
    PRIVATE
        $<TARGET_PROPERTY:service-desktop,INCLUDE_DIRECTORIES>
)
