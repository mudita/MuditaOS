/*
 * @file NotesMainWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 31 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_NOTES_WINDOWS_NOTESMAINWINDOW_HPP_
#define MODULE_APPS_APPLICATION_NOTES_WINDOWS_NOTESMAINWINDOW_HPP_

#include <string>
#include <functional>

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

namespace gui {

class NotesMainWindow: public AppWindow {
public:
	NotesMainWindow( app::Application* app );
	virtual ~NotesMainWindow();

	//virtual methods
	bool onInput( const InputEvent& inputEvent ) override;
	void onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) override;

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_NOTES_WINDOWS_NOTESMAINWINDOW_HPP_ */
