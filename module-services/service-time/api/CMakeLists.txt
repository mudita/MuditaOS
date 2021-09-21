add_library(service-time-api STATIC)

target_sources(service-time-api
        PRIVATE
        TimeSettingsApi.cpp
        PUBLIC
        TimeSettingsApi.hpp
        )

target_link_libraries(service-time-api
        PRIVATE
        utils-time
        service-time
        )
