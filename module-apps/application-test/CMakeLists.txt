project(application-test)
add_library(${PROJECT_NAME} STATIC)

target_include_directories(${PROJECT_NAME}
        PRIVATE
                $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/>
        PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_sources(${PROJECT_NAME}
        PRIVATE
                ApplicationTest.cpp
                windows/TestWindow.cpp
                windows/TestPopup.cpp
                presenters/TestPresenter.cpp
        PUBLIC
                include/application-test/ApplicationTest.hpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    PUBLIC
        apps-common
        module-gui
    )
