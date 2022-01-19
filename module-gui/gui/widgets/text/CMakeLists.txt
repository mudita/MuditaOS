target_sources( ${PROJECT_NAME}
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/core/cursors/TextBlockCursor.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/core/cursors/TextCursor.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/core/cursors/TextInLineCursor.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/core/cursors/TextLinesCursor.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/core/lines/Lines.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/core/lines/TextLine.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/core/lines/MultiTextLine.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/core/lines/SingleTextLine.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/core/RawText.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/core/TextBlock.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/core/TextDocument.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/core/TextFormat.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/modes/InputMode.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/parsers/RichTextParser.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/parsers/TextParse.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Label.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Text.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/TextBubble.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/TextFixedSize.cpp"

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/Text.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/TextFixedSize.hpp"
)
