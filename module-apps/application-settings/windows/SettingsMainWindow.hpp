/*
 * @file SettingsMainWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_SETTINGS_WINDOWS_SETTINGSMAINWINDOW_HPP_
#define MODULE_APPS_APPLICATION_SETTINGS_WINDOWS_SETTINGSMAINWINDOW_HPP_

#include <string>
#include <functional>

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

namespace gui {

/*
 *
 */
class SettingsMainWindow: public AppWindow {
protected:
	gui::Label* title;
	std::vector<gui::Item*> options;

	gui::Item* addOptionLabel( const std::string& text, std::function<bool(Item&)> activatedCallback );
public:
	SettingsMainWindow( app::Application* app );
	virtual ~SettingsMainWindow();

	//virtual methods
	void onBeforeShow( ShowMode mode, SwitchData* data ) override;

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_SETTINGS_WINDOWS_SETTINGSMAINWINDOW_HPP_ */
