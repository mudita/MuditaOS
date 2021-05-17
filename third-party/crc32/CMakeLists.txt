
add_library(crc32)
target_sources(crc32
   PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src/crc32.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/crc32.h
)

target_include_directories(crc32
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

