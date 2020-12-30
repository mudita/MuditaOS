project(service-cellular)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
    CellularCall.cpp
    CellularServiceAPI.cpp
    CellularUrcHandler.cpp
    checkSmsCenter.cpp
    ServiceCellular.cpp
    SignalStrength.cpp
    SimCard.cpp
    NetworkSettings.cpp
    PacketData.cpp
    QMBNManager.cpp
    RequestFactory.cpp
    CellularRequestHandler.cpp
    requests/Request.cpp
    requests/CallRequest.cpp
    requests/SupplementaryServicesRequest.cpp
    requests/CallForwardingRequest.cpp
    requests/PasswordRegistrationRequest.cpp
    requests/PinChangeRequest.cpp
    requests/ImeiRequest.cpp
    requests/UssdRequest.cpp
    requests/ClipRequest.cpp
    requests/ClirRequest.cpp
    requests/ColpRequest.cpp
    requests/CallWaitingRequest.cpp
    requests/CallBarringRequest.cpp)


add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        service-antenna
        service-audio
        service-evtmgr
        module-bsp
        module-cellular
    )

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
