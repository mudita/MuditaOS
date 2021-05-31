add_library(clipboard STATIC)
target_sources(clipboard
   PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/Clipboard.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/Clipboard.cpp
)

target_include_directories(clipboard
   PUBLIC
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(clipboard
   PUBLIC
       module-os
)

if (${ENABLE_TESTS})
   add_subdirectory(test)
endif()
