add_library(sys-phonemodes STATIC)

target_sources(sys-phonemodes
    PUBLIC
        include/PhoneModes/Common.hpp
        include/PhoneModes/Observer.hpp
        include/PhoneModes/Subject.hpp
        
    PRIVATE
        Observer.cpp
        Subject.cpp
)

target_include_directories(sys-phonemodes
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(sys-phonemodes
    PUBLIC
        sys-service
)
