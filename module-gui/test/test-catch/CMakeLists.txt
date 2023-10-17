# gui tests
add_catch2_executable(
        NAME
                gui
        SRCS
                test-gui.cpp
                test-context.cpp
                test-gui-callbacks.cpp
                test-gui-resizes.cpp
                test-gui-image.cpp
                ../mock/TestWindow.cpp
                test-language-input-parser.cpp
                test-key-translator.cpp
        INCLUDE
                ..
        LIBS
                module-sys
                module-gui
                gui-mock
	USE_FS
)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/plus_32px_W_M.vpi" DESTINATION "${CMAKE_BINARY_DIR}/images")
