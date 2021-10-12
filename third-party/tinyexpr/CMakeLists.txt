
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

target_compile_options( tinyexpr
	PRIVATE
	-Wno-array-bounds
)

add_library(tinyexpr::tinyexpr ALIAS tinyexpr)

