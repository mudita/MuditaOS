add_library(gui-mock
    OBJECT
        InitializedFontManager.cpp)

target_link_libraries(gui-mock
    PRIVATE
        module-gui)

target_include_directories(gui-mock
   PUBLIC
        ..)
