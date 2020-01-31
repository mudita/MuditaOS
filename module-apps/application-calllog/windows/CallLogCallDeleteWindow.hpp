/*
 * @file CallLogCallDeleteWindow.hpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 14.10.2019
 * @brief Delete Window
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

#include "CalllogRecord.hpp"

namespace gui {

class CallLogCallDeleteWindow: public AppWindow {
protected:
	gui::Text* questionText = nullptr;
	gui::Image* trashImg = nullptr;

	std::vector<gui::Item*> options;

	gui::Item* addOptionLabel( const std::string& text, std::function<bool(Item&)> activatedCallback );

	CalllogRecord record = {0}; 
public:
	CallLogCallDeleteWindow( app::Application* app );
	virtual ~CallLogCallDeleteWindow();

	//virtual methods
	void onBeforeShow( ShowMode mode, SwitchData* data ) override;

	void rebuild() override;
	void buildInterface() override;
	void destroyInterface() override;

    bool removeCalllogEntry(const CalllogRecord *record);
};

} /* namespace gui */
