# gui tests

set(PROPRIETARY_SOURCES "")
if (${ASSETS_TYPE} STREQUAL "Propertiary")
        list(APPEND PROPRIETARY_SOURCES
                test-gui-Text.cpp
                test-gui-TextFixedSize.cpp
                test-gui-Font.cpp
                test-gui-TextLinesCursor.cpp
                test-gui-TextInLineCursor.cpp
        )
endif()

add_catch2_executable(
        NAME
                gui-text
        SRCS
                ../mock/buildTextDocument.cpp
                ../mock/multi-line-string.cpp
                ../mock/InitializedFontManager.cpp
                test-gui-TextBlock.cpp
                test-gui-TextBlockCursor.cpp
                test-gui-TextDocument.cpp
                test-gui-MultiTextLine.cpp
                test-gui-TextParse.cpp
                ${PROPRIETARY_SOURCES}

                INCLUDE
                ..
                ../mock/
        LIBS
                module-sys
                module-gui
	USE_FS
)
