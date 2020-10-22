project(service-cellular)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

set(SOURCES
    CellularCall.cpp
    CellularServiceAPI.cpp
    CellularUrcHandler.cpp
    ServiceCellular.cpp
    SignalStrength.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

	PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        service-antenna
        service-audio
        module-bsp
        module-cellular
    )

