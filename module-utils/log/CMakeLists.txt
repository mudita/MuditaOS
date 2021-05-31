add_library(log STATIC)
target_sources(log
   PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/LoggerBuffer.cpp
)
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
target_include_directories(log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
        target_link_libraries(log PRIVATE module-bsp)
#endif()
=======
target_link_libraries(log PUBLIC module-bsp)
>>>>>>> dde20ca8b... [BH-378] Utils move log to library
=======
=======
target_include_directories(log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/.)
>>>>>>> 60c9dd9ef... [BH-378] After rebase
target_link_libraries(log PRIVATE module-bsp)
>>>>>>> ca852003d... [BH-378] PR changes
