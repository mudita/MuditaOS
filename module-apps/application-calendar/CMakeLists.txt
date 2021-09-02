# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-calendar STATIC)

target_include_directories(application-calendar
	PRIVATE
		$<BUILD_INTERFACE:
			${CMAKE_CURRENT_SOURCE_DIR}
			data
			include/
			models
			widgets
			windows
		>
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-calendar

	PRIVATE
		ApplicationCalendar.cpp
		data/CalendarData.cpp
		models/MonthModel.cpp
		widgets/DayLabel.cpp
		widgets/MonthBox.cpp
		windows/CalendarMainWindow.cpp

	PRIVATE
		models/MonthModel.hpp
		widgets/DayLabel.hpp
		widgets/MonthBox.hpp
		windows/CalendarMainWindow.hpp
		
	PUBLIC
		include/application-calendar/ApplicationCalendar.hpp
		include/application-calendar/data/CalendarData.hpp
		include/application-calendar/data/OptionParser.hpp
)

target_link_libraries(application-calendar
	PUBLIC
		apps-common
		date::date
		module-gui
		module-sys
		module-utils
		time-constants

	PRIVATE
		service-appmgr
		service-db
		service-time
)
