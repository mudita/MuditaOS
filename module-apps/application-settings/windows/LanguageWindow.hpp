/*
 * @file LanguageWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 9 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_SETTINGS_WINDOWS_LANGUAGEWINDOW_HPP_
#define MODULE_APPS_APPLICATION_SETTINGS_WINDOWS_LANGUAGEWINDOW_HPP_

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
class LanguageWindow: public AppWindow {
protected:
	std::vector<gui::Item*> options;

	gui::Item* addOptionLabel( const std::string& text, std::function<bool(Item&)> activatedCallback );
public:
	LanguageWindow( app::Application* app );
	virtual ~LanguageWindow();

	//virtual methods
	void onBeforeShow( ShowMode mode, SwitchData* data ) override;

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;
};
} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_SETTINGS_WINDOWS_LANGUAGEWINDOW_HPP_ */
