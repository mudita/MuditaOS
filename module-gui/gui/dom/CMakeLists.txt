target_sources( ${PROJECT_NAME}
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/Item2JsonSerializingVisitor.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Item2JsonSerializer.cpp"

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/Item2JsonSerializingVisitor.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Item2JsonSerializer.hpp"
)
