project(bluetooth_uml)
add_executable(${PROJECT_NAME} uml_printer.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
        module-bluetooth
        log
        sml::sml
        sml::utils::logger
)
