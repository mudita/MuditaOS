/*
 * @file CallLogOptionsWindow.hpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 10.10.2019
 * @brief Call Log Options Window
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

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

#include "../data/CallLogInternals.hpp"

namespace gui {

class CallLogOptionsWindow: public AppWindow {
protected:
	std::vector<gui::Item*> options;

	gui::Item* addOptionLabel( const std::string& text, std::function<bool(Item&)> activatedCallback );

    CalllogRecord record;

  public:
	CallLogOptionsWindow( app::Application* app );
	virtual ~CallLogOptionsWindow();

	//virtual methods
	void onBeforeShow( ShowMode mode, SwitchData* data ) override;

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;
};

} /* namespace gui */

