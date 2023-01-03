add_library(databases STATIC)
add_library(bell::db ALIAS databases)

add_subdirectory(databases)

target_sources(databases
    PRIVATE
        ServiceDB.cpp
        agents/MeditationStatsAgent.cpp
    PUBLIC
        include/db/ServiceDB.hpp
        include/db/SystemSettings.hpp
)

target_include_directories(databases
    PUBLIC
        include
)

target_link_libraries(databases
   PRIVATE
        bell::db::meditation_stats
        service-db
        serial-number-reader
        crashdump-serial-number
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
