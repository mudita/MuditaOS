/*
 * @file MenuWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 29 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_MENUWINDOW_HPP_
#define MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_MENUWINDOW_HPP_

#include <vector>

#include "AppWindow.hpp"
#include "widgets/BottomBar.hpp"
#include "widgets/TopBar.hpp"
#include "widgets/Label.hpp"

namespace gui {

/*
 *
 */
class MenuWindow: public AppWindow {
	//name of icon, description, name of application to run
	struct TileStrings{
		std::string iconName;
		std::string i18Desctiption;
		std::string applicationName;
	};
	std::vector<TileStrings> tileDefinitions {
		TileStrings{"menu_phone","app_desktop_menu_phone","Phonebook"},
		TileStrings{"menu_contacts","app_desktop_menu_contacts","Contacts"},
		TileStrings{"menu_messages","app_desktop_menu_messages","Messages"},
		TileStrings{"menu_calendar","app_desktop_menu_calendar","Calendar"},
		TileStrings{"menu_alarm","app_desktop_menu_alarm","Alarm"},
		TileStrings{"menu_meditation","app_desktop_menu_meditation","Meditation"},
		TileStrings{"menu_music_player","app_desktop_menu_music","Music"},
		TileStrings{"menu_tools","app_desktop_menu_tools","Tools"},
		TileStrings{"menu_settings","app_desktop_menu_settings","Settings"},
			//TODO this is for tests of menu pages
//		TileStrings{"menu_alarm","app_desktop_menu_meditation","Meditation"},
//		TileStrings{"menu_calculator","app_desktop_menu_calculator","Calculator"},
//		TileStrings{"menu_settings","app_desktop_menu_settings","Settings"},
//		TileStrings{"menu_settings","app_desktop_menu_settings","Settings"}
	};
protected:
	//page that is currently selected by the user.
	uint32_t currentPage = 0;
	uint32_t pageSize = 9;
	gui::BottomBar* bottomBar = nullptr;
	gui::TopBar* topBar = nullptr;
	gui::Label* title = nullptr;
	std::vector<gui::Item*> tiles;

public:
	MenuWindow( app::Application* app );
	virtual ~MenuWindow();

	void onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) override;
	bool onInput( const InputEvent& inputEvent ) override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_DESKTOP_WINDOWS_MENUWINDOW_HPP_ */
