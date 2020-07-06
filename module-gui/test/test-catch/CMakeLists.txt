# gui tests
add_catch2_executable(
        NAME
                gui
        SRCS
                test-gui.cpp
                test-context.cpp
                test-gui-callbacks.cpp
                test-gui-resizes.cpp
                ../mock/TestWindow.cpp
                ../mock/InitializedFontManager.cpp
        INCLUDE
                ..
        LIBS
                module-gui
        DEPS
                assets
)
