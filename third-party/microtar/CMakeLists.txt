add_library(microtar)
target_sources(microtar
   PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src/src/microtar.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/src/microtar.hpp
)

target_include_directories(microtar
   PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/src>
)
