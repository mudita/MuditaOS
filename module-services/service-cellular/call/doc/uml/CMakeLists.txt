project(call_uml)
add_executable(${PROJECT_NAME} uml_printer.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
        log
        module-cellular
        service-cellular-call
        sml::sml
        sml::utils::logger
        test::fakeit
)
