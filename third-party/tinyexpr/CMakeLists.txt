
add_library(tinyexpr)
target_sources(tinyexpr
   PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src/tinyexpr.c
      ${CMAKE_CURRENT_SOURCE_DIR}/src/tinyexpr.h
)

target_include_directories(tinyexpr
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
add_library(tinyexpr::tinyexpr ALIAS tinyexpr)

