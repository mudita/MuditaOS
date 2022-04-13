project(service-cellular-call)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

add_library(${PROJECT_NAME} STATIC
    CellularCall.cpp
    api/CallAudio.cpp
    api/CallGUI.cpp
    api/CallDB.cpp
    api/CallMulticast.cpp
    api/CallTimer.cpp
    api/ModemCallApi.cpp
)
set_source_files_properties(
        CellularCall.cpp
        PROPERTIES COMPILE_FLAGS
        "-Wno-error=unused-but-set-variable"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    PUBLIC
        ./
        ./include/
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        module-sys
        sml::utils::logger
    PUBLIC
        libphonenumber::libphonenumber
    )

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    add_subdirectory(doc/uml/)
endif()

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
