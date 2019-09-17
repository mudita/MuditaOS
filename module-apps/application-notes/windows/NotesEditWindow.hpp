/*
 * @file NotesEditWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 13 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_NOTES_WINDOWS_NOTESEDITWINDOW_HPP_
#define MODULE_APPS_APPLICATION_NOTES_WINDOWS_NOTESEDITWINDOW_HPP_

#include <string>
#include <functional>

#include "AppWindow.hpp"
#include "gui/widgets/Text.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"
#include "gui/widgets/ListView.hpp"

namespace gui {

class NotesEditWindow: public AppWindow {
	Text* text = nullptr;
	Label* title = nullptr;
public:
	NotesEditWindow( app::Application* app );
	virtual ~NotesEditWindow();

	//virtual methods
	bool onInput( const InputEvent& inputEvent ) override;
	void onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) override;

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_NOTES_WINDOWS_NOTESEDITWINDOW_HPP_ */
