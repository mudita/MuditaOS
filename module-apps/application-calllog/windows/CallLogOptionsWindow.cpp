/*
 * @file CallLogOptionsWindow.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 10.10.2019
 * @brief Call Log Options Window
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "CallLogOptionsWindow.hpp"

#include <memory>
#include <functional>

#include "service-appmgr/ApplicationManager.hpp"

#include "bsp/rtc/rtc.hpp"

#include "../ApplicationCallLog.hpp"

#include "service-db/messages/DBMessage.hpp"
#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "time/time_conversion.hpp"
#include "../data/CallLogSwitchData.hpp"

#include <Style.hpp>

using namespace callLogStyle::optionsWindow;

namespace gui
{

CallLogOptionsWindow::CallLogOptionsWindow( app::Application* app ) : AppWindow(app,calllog::settings::OptionsWindowStr){
	buildInterface();
}

void CallLogOptionsWindow::rebuild() {
	//find which widget has focus
	uint32_t index = 0; // in case of not find will be 0
	std::vector<gui::Item*>::iterator it = std::find_if(options.begin(), options.end(), [=](auto val){ return val == getFocusItem();});
	if (it != options.end()) {
		index = distance(options.begin(), it);
	} 
	
	destroyInterface();
	buildInterface();
	setFocusItem( options[index] );
}
void CallLogOptionsWindow::buildInterface() {
	AppWindow::buildInterface();

	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_select"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	topBar->setActive(TopBar::Elements::TIME, true );

	setTitle(utils::localize.get("app_calllog_options_title"));

	//add option contact details option
	options.push_back( addOptionLabel( utils::localize.get("app_calllog_options_contact_details"), 
			[=](gui::Item&){ return true;}) ); // TODO: alek: contact details support

	//add option delete call option
	options.push_back( addOptionLabel( utils::localize.get("app_calllog_options_delete_call"),
			[=] (gui::Item& item){ 
				std::unique_ptr<gui::SwitchData> data = std::make_unique<calllog::CallLogSwitchData>(record);
				application->switchWindow(calllog::settings::CallDeleteWindowStr, std::move(data));
				return true;
			}) );

	//set position and navigation for labels
	auto posY = title->offset_h();
	auto size = options.size();
	for (size_t i = 0; i < size; i++){
		options[i]->setPosition(option::x,posY); // TODO: alek:: sthg is wrong here
		posY += option::offset;
		options[i]->setNavigationItem( NavigationDirection::DOWN, options[(i+1)%size]);
		options[i]->setNavigationItem( NavigationDirection::UP, options[(size+i-1)%size]);
	}
}

void CallLogOptionsWindow::destroyInterface() {
	AppWindow::destroyInterface();
	for(size_t i=0; i<options.size(); i++){
		if(options[i]!=nullptr) { removeWidget(options[i]); delete options[i]; options[i] = nullptr; }
	}
	options.clear();
	this->focusItem = nullptr;
	children.clear();
}

CallLogOptionsWindow::~CallLogOptionsWindow() {
	destroyInterface();
}

gui::Item* CallLogOptionsWindow::addOptionLabel( const std::string& text, std::function<bool(Item&)> activatedCallback ) {
	gui::Label *label = new gui::Label(this, 0, 0, 0, 0, text); 
	style::window::decorateOption(label);
	label->activatedCallback = activatedCallback;

	return label;
}


void CallLogOptionsWindow::onBeforeShow( ShowMode mode, SwitchData* data ) {
	if( mode == ShowMode::GUI_SHOW_INIT) setFocusItem(options.front());

	if(data != nullptr && data->getDescription() == calllog::CALLLOG_SWITCH_DATA_STR) {
		auto switchData = reinterpret_cast<calllog::CallLogSwitchData*>(data);	
		record = switchData->getRecord();
	}
}

} /* namespace gui */
