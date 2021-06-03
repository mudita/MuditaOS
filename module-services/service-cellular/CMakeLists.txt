project(service-cellular)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
    PRIVATE
    src/ServiceCellularPriv.cpp
    src/Settings.cpp
    src/State.cpp
    src/static-api.cpp
    src/SimCard.cpp

    PRIVATE
    src/ServiceCellularPriv.hpp
    src/messages.hpp
    src/Settings.hpp
    src/static-api.hpp
    src/SimCard.hpp

    PRIVATE
    CellularCall.cpp
    CellularServiceAPI.cpp
    CellularUrcHandler.cpp
    checkSmsCenter.cpp
    ServiceCellular.cpp
    SignalStrength.cpp
    NetworkSettings.cpp
    PacketData.cpp
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
    handler/RawATHandler.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src
)

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
    PUBLIC
        time
        utf8
        eventstore
    )

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
