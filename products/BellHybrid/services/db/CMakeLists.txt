add_library(databases STATIC)
add_library(bell::db ALIAS databases)

add_subdirectory(databases)

target_sources(databases
    PRIVATE
        ServiceDB.cpp
        BellFactorySettings.cpp
        agents/MeditationStatsAgent.cpp
        agents/QuotesAgent.cpp
        agents/ShuffleQuoteModel.cpp
        agents/WhatsNewAgent.cpp
        agents/QuotesAgent.hpp
        agents/ShuffleQuoteModel.hpp
        agents/WhatsNewAgent.hpp
    PUBLIC
        include/db/ServiceDB.hpp
        include/db/SystemSettings.hpp
        include/db/BellFactorySettings.hpp
)

target_include_directories(databases
    PUBLIC
        include
        agents
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
