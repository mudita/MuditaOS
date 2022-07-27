project(service-cellular)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
    src/ServiceCellularPriv.cpp
    src/State.cpp
    src/SimCard.cpp
    src/NetworkTime.cpp
    src/SimContacts.cpp
    src/SMSPartsHandler.cpp
    src/SMSSendHandler.cpp
    src/ImeiGetHandler.cpp
    src/TerhetingHandler.cpp
    src/ModemResetHandler.cpp
    src/URCCounter.cpp
    src/CSQHandler.cpp
        DTMFCode.cpp

    CellularServiceAPI.cpp
    CellularUrcHandler.cpp
    checkSmsCenter.cpp
    ServiceCellular.cpp
    SignalStrength.cpp
    NetworkSettings.cpp
    PacketData.cpp
    Pdu.cpp
    QMBNManager.cpp
    RequestFactory.cpp
    CellularRequestHandler.cpp
    connection-manager/ConnectionManager.cpp
    connection-manager/ConnectionManagerCellularCommands.cpp
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
    requests/CallBarringRequest.cpp
    handler/RawATHandler.cpp)


add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src
)

add_subdirectory(call)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Microsoft.GSL::GSL
        module-bsp
        module-cellular
        re2::re2
        service-antenna
        service-audio
        service-evtmgr
        ucs2
        json::json
        utils-time
    PUBLIC
        service-cellular-call
        eventstore
        utf8
        hal
    )

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
