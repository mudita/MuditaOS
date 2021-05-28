#module_is_test_entity()

if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
	include(targets/Target_Linux.cmake)
else()
	include(targets/Target_Cross.cmake)
endif()

add_library(board STATIC)
target_sources(board
   PRIVATE
       ${SOURCES}
       ${BOARD_SOURCES}
)
target_link_libraries(board PUBLIC module-bsp)

#target_include_directories(board PUBLIC ${BOARD_DIR_INCLUDES} )

#include(segger/rtt/CMakeLists.txt)

# Board specific compilation definitions,options,include directories and features
#target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
#target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
#target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
#target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES} )
#target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
#target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
#target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
#target_compile_options( ${PROJECT_NAME} PUBLIC "-Wno-error=deprecated-declarations")
#target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})
