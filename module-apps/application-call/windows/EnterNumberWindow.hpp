/*
 * @file EnterNumberWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"
#include "../widgets/Icon.hpp"

namespace gui {

/*
 *
 */
class EnterNumberWindow: public AppWindow {
protected:
	gui::Label* numberLabel = nullptr;
    gui::Icon * newContactIcon = nullptr;

    void updateBottomBar();

    void setNumberLabel(const std::string num);

  public:
	EnterNumberWindow( app::Application* app, std::string windowName = "EnterNumberWindow" );
	virtual ~EnterNumberWindow();

	bool onInput( const InputEvent& inputEvent ) override;
	bool handleSwitchData( SwitchData* data ) override;

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;
};

} /* namespace gui */

