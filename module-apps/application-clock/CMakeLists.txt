
add_library(application-clock STATIC)

target_include_directories(application-clock

	PRIVATE
		$<BUILD_INTERFACE:
			${CMAKE_CURRENT_LIST_DIR}/windows
		>

	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_sources(application-clock

	PRIVATE
		ApplicationClock.cpp
		windows/ClockMainWindow.cpp
	PUBLIC
		include/ApplicationClock.hpp
		windows/ClockMainWindow.hpp
)

option(ENABLE_APP_CLOCK "Enable application clock" ON)

target_compile_definitions(application-clock
    INTERFACE
        $<$<BOOL:${ENABLE_APP_CLOCK}>:ENABLE_APP_CLOCK>
)

target_link_libraries(application-clock
    PRIVATE
       service-evtmgr
	   apps-common
    )

