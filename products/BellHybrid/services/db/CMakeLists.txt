add_library(databases STATIC)
add_library(bell::db ALIAS databases)

add_subdirectory(databases)

target_sources(databases
    PRIVATE
        ServiceDB.cpp
        BellFactorySettings.cpp
        agents/MeditationStatsAgent.cpp
    PUBLIC
        include/db/ServiceDB.hpp
        include/db/SystemSettings.hpp
        include/db/BellFactorySettings.hpp
)

target_include_directories(databases
    PUBLIC
        include
)

target_link_libraries(databases
   PRIVATE
        bell::db::meditation_stats
        service-db
        serial-number-parser
        crashdump-metadata-store
)

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
