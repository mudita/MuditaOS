/*
 * @file CallLogCallDeleteWindow.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 14.10.2019
 * @brief Delete Window
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "CallLogCallDeleteWindow.hpp"
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
#include "service-db/api/DBServiceAPI.hpp"
#include "../data/CallLogSwitchData.hpp"
#include "../data/CallLogInternals.hpp" // TODO: alek: add easier paths
#include <Style.hpp>

namespace gui {

using namespace callLogStyle::deleteWindow;

CallLogCallDeleteWindow::CallLogCallDeleteWindow( app::Application* app ) : AppWindow(app,calllog::settings::CallDeleteWindowStr){
	buildInterface();
}

void CallLogCallDeleteWindow::rebuild() {
	//find which widget has focus
	uint32_t index = 0; // in case of doesn't find, will be 0
	std::vector<gui::Item*>::iterator it = std::find_if(options.begin(), options.end(), [=](auto val){ return val == getFocusItem();});
	if (it != options.end()) {
		index = distance(options.begin(), it);
	} 

	destroyInterface();
	buildInterface();
	setFocusItem( options[index] );
}
void CallLogCallDeleteWindow::buildInterface() {
	AppWindow::buildInterface();

	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_confirm"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	topBar->setActive(TopBar::Elements::TIME, true );

	// trashImg =  // TODO: add this

	questionText = new gui::Text(this, question::x, question::y, question::w, question::h);
	questionText->setFilled( false );
	questionText->setBorderColor( gui::ColorNoColor );
	questionText->setTextType( Text::TextType::MULTI_LINE );
	questionText->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
	questionText->setFont(style::window::font::medium);
	questionText->setText(utils::localize.get("app_calllog_delete_call_confirmation"));
	// TODO: alek: set alignment

	//add option connectivity option
	options.push_back( addOptionLabel( utils::localize.get("common_no"), 
		[=] (gui::Item& item){ 
			application->switchWindow( calllog::settings::OptionsWindowStr, gui::ShowMode::GUI_SHOW_RETURN );
			return true; 
		}) );

	//add option date and time option
	options.push_back( addOptionLabel( utils::localize.get("common_yes"), 
		[=](gui::Item&){ 
			DBServiceAPI::CalllogRemove(application, record.id);
			application->switchWindow( calllog::settings::MainWindowStr);
			return true;}) );

	//set position and navigation for labels
	
	uint32_t size = options.size();
	// 'No' Label
	options[0]->setPosition(options::no::x,options::y);
	// 'Yes' Label
	options[1]->setPosition(options::yes::x,options::y);;

	for( uint32_t i=0; i<options.size(); i++ ){
		options[i]->setNavigationItem( NavigationDirection::LEFT, options[(i+1)%size]);
		options[i]->setNavigationItem( NavigationDirection::RIGHT, options[(size+i-1)%size]);
	}
}

void CallLogCallDeleteWindow::destroyInterface() {
	AppWindow::destroyInterface();
	if( questionText ) { removeWidget(questionText); delete questionText; questionText = nullptr; }
	if( trashImg ) { removeWidget(trashImg); delete trashImg; trashImg = nullptr; }
	for( uint32_t i=0; i<options.size(); i++ ){
		if( options[i] ) { removeWidget(options[i]); delete options[i]; options[i] = nullptr; }
	}
	options.clear();
	this->focusItem = nullptr;
	LOG_INFO("options size: %d", options.size());
	children.clear();
}

CallLogCallDeleteWindow::~CallLogCallDeleteWindow() {
	destroyInterface();
}

gui::Item* CallLogCallDeleteWindow::addOptionLabel( const std::string& text, std::function<bool(Item&)> activatedCallback ) {
	gui::Label* label = new gui::Label( this, 0,0, options::w, options::h, text ); 
    style::window::decorate(label);
	label->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
    label->activatedCallback = activatedCallback;
	return label;
}


void CallLogCallDeleteWindow::onBeforeShow( ShowMode mode, SwitchData* data ) {
	if( mode == ShowMode::GUI_SHOW_INIT ) setFocusItem( options[0] ); // TODO: alek: change index to type

	if(data != nullptr && data->getDescription() == calllog::CALLLOG_SWITCH_DATA_STR) {
		auto switchData = reinterpret_cast<calllog::CallLogSwitchData*>(data);	
		record = switchData->getRecord();

		setTitle(record.name);
	}
}

} /* namespace gui */


