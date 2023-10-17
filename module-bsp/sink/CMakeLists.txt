add_library(system-stats-sink-interface INTERFACE)
target_include_directories(system-stats-sink-interface INTERFACE ./)

add_library(system-stats-sink)

target_sources(system-stats-sink
   PRIVATE
      FileSink.cpp
   PUBLIC
      ./
)

target_link_libraries(
        system-stats-sink
    PRIVATE
        system-stats-sink-interface
        system-stats-sink-board
)

target_include_directories(system-stats-sink
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
)
