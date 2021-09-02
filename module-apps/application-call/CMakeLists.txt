# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-call STATIC)

target_include_directories(application-call
	PRIVATE
		$<BUILD_INTERFACE:
			${CMAKE_CURRENT_SOURCE_DIR}/data
			${CMAKE_CURRENT_SOURCE_DIR}/widgets
			${CMAKE_CURRENT_SOURCE_DIR}/windows
			${CMAKE_CURRENT_SOURCE_DIR}/include/application-call
		>
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-call
	PRIVATE
		ApplicationCall.cpp
		data/CallAppStyle.hpp
		data/CallSwitchData.hpp
		widgets/StateIcon.hpp
		widgets/StateIcons.cpp
		widgets/StateIcons.hpp
		windows/CallWindow.cpp
		windows/CallWindow.hpp
		windows/EmergencyCallWindow.cpp
		windows/EmergencyCallWindow.hpp
		windows/EnterNumberWindow.cpp
		windows/EnterNumberWindow.hpp
		windows/NumberWindow.cpp
		windows/NumberWindow.hpp
	PUBLIC
		include/application-call/ApplicationCall.hpp
		include/application-call/CallState.hpp
)

target_link_libraries(application-call
    PRIVATE
		i18n
		libphonenumber::libphonenumber
		log
		magic_enum
		module-gui
		module-utils
		service-appmgr
		service-audio
		service-db
		time
	PUBLIC
		apps-common
		module-sys
		service-cellular
		service-evtmgr
)
