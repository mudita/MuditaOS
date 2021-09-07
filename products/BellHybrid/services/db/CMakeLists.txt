add_library(db STATIC)
add_library(bell::db ALIAS db)

target_sources(db
    PRIVATE
        ServiceDB.cpp
    PUBLIC
        include/db/ServiceDB.hpp
        include/db/SystemSettings.hpp
)

target_include_directories(db
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(db
   PRIVATE
        module-db
        service-db
)
