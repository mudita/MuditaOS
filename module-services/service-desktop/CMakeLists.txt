message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )
if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
	add_subdirectory( rt1051 )
endif()

if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
	add_subdirectory( linux )
endif()

add_subdirectory( parser )


target_compile_definitions (${PROJECT_NAME} PUBLIC USB_STACK_FREERTOS)
target_include_directories (${PROJECT_NAME} PUBLIC usb/include usb/osa usb/device/class usb/device/include)

target_sources( ${PROJECT_NAME}
    PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/ServiceDesktop.cpp"
)
