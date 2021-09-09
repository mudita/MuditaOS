add_library(application-special-input STATIC)

target_include_directories(application-special-input
        PRIVATE
                $<BUILD_INTERFACE:
                        ${CMAKE_CURRENT_LIST_DIR}/data
                        ${CMAKE_CURRENT_LIST_DIR}/include/application-special-input
                        ${CMAKE_CURRENT_LIST_DIR}/models
                        ${CMAKE_CURRENT_LIST_DIR}/widgets
                        ${CMAKE_CURRENT_LIST_DIR}/windows
                >
        PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_sources(application-special-input
        PRIVATE
                ApplicationSpecialInput.cpp
                models/SpecialInputModel.cpp
                models/SpecialInputModel.hpp
                windows/SpecialInputMainWindow.cpp
                windows/SpecialInputMainWindow.hpp
                widgets/SpecialInputTableWidget.cpp
                widgets/SpecialInputTableWidget.hpp

        PUBLIC
                include/application-special-input/ApplicationSpecialInput.hpp
)

target_link_libraries(application-special-input
    PRIVATE
        app
        apps-common
        i18n
        service-appmgr
        utf8

    PUBLIC
        apps-common
        module-gui
    )
