add_library(microtar)
add_library(microtar::microtar ALIAS microtar)
target_sources(microtar
   PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src/src/microtar.cpp

   PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/src/src/microtar.hpp
)

target_include_directories(microtar
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/src>
)
