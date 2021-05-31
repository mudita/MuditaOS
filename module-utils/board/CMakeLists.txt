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
