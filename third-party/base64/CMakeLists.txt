
add_library(base64)
add_library(base64::base64 ALIAS base64)
target_sources(base64
   PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src/base64.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/base64.h
)

target_include_directories(base64
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

