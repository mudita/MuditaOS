project(service-cellular-call)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

add_library(${PROJECT_NAME} STATIC 
    CellularCall.cpp
    CallAudio.cpp
    CallGUI.cpp
    CallDB.cpp
    CallMulticast.cpp
    CallRingGuard.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ./include/
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        module-sys
    )

# uncomment to add tests:
# if (${ENABLE_TESTS})
#     add_subdirectory(tests)
# endif ()
