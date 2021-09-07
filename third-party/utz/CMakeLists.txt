
add_library(utz)
add_library(utz::utz ALIAS utz)
target_sources(utz
   PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src/utz/utz.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/utz/zones.c
   PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/src/utz/utz.h
      ${CMAKE_CURRENT_SOURCE_DIR}/src/utz/zones.h
)

target_include_directories(utz
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

