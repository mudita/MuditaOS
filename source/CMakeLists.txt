add_library(messagetype INTERFACE)
target_sources(messagetype
    PUBLIC
        MessageType.hpp
    )
target_include_directories(messagetype INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
