add_catch2_executable(
        NAME
        bell-db
        SRCS
        MeditationStatisticsTable_tests.cpp

        LIBS
        bell::db::meditation_stats
        USE_FS
)