link_directories(
    ${GTKMM_LIBRARY_DIRS}  )

include_directories(
    ${GTKMM_INCLUDE_DIRS}  )
   
add_executable(gui_renderer # name of the executable on Windows will be example.exe 
    gui_renderer.cpp 
    RArea.cpp
    RWindow.cpp
    RArea.hpp
    RWindow.hpp )

target_link_libraries(gui_renderer 
    ${GTKMM_LIBRARIES}  )
    
find_library(LIBRT rt) 
if(LIBRT)
	target_link_libraries(gui_renderer ${LIBRT})
endif()
