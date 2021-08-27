add_library(eventstore STATIC)
target_sources(eventstore
   PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/EventStore.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/EventStore.cpp
)

target_include_directories(eventstore
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(eventstore
   PRIVATE
      module-os
      log-api
)

