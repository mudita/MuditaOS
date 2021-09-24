# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-calllog STATIC)

target_include_directories( application-calllog

	PRIVATE
		$<BUILD_INTERFACE:
			${CMAKE_CURRENT_SOURCE_DIR}/data
			${CMAKE_CURRENT_SOURCE_DIR}/include/application-calllog
			${CMAKE_CURRENT_SOURCE_DIR}/widgets
			${CMAKE_CURRENT_SOURCE_DIR}/windows
		>
	PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources( application-calllog

	PRIVATE
		ApplicationCallLog.cpp
		CalllogModel.hpp
		CalllogModel.cpp
		data/CallLogSwitchData.hpp
		windows/CallLogDetailsWindow.hpp
		windows/CallLogDetailsWindow.cpp
		widgets/CalllogItem.cpp
		windows/CallLogMainWindow.hpp
		windows/CallLogMainWindow.cpp
		windows/CallLogOptionsWindow.hpp
		windows/CallLogOptionsWindow.cpp

	PUBLIC
		include/application-calllog/ApplicationCallLog.hpp
)

target_link_libraries( application-calllog

	PRIVATE
		app
		i18n
		log
		Microsoft.GSL::GSL
		module-apps
		module-gui
		module-utils
		service-appmgr
		service-db
		utils-time
	PUBLIC
		apps-common
		module-db
)
