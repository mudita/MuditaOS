add_library(meditation_stats_db
        MeditationStatisticsTable.cpp
        MeditationStatisticsDB.cpp
        )

add_library(bell::db::meditation_stats ALIAS meditation_stats_db)

target_link_libraries(meditation_stats_db
        PUBLIC
        module-db
        )

target_include_directories(meditation_stats_db PUBLIC .)