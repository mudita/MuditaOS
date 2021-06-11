add_library(log STATIC)
target_sources(log
   PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/LoggerBuffer.cpp
)
target_include_directories(log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(log PRIVATE module-bsp)
