target_sources( ${PROJECT_NAME}

    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/DepthFirstItemTree.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/DepthFirstItemTree.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Item2JsonSerializingVisitor.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Item2PrettyPrintSerializingVisitor.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/ItemDomCreator.cpp"

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/GuiVisitor.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/ItemTree.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/DepthFirstItemTree.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Item2JsonSerializingVisitor.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Item2PrettyPrintSerializingVisitor.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/ItemDomCreator.hpp"
)
