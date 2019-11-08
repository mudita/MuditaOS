/*
 * @file CallLogDetailsWindow.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 05.10.2019
 * @brief Call Log Details Window
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "CallLogDetailsWindow.hpp"
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
#include "../data/CallLogInternals.hpp" // TODO: alek: add easier paths

#include <Style.hpp>

using namespace calllog;

namespace gui {

CallLogDetailsWindow::CallLogDetailsWindow( app::Application* app ) :
	AppWindow(app, calllog::settings::DetailsWindowStr), leftMargin(style::window::default_left_margin), rightMargin(style::window::default_right_margin) {

	buildInterface();
}

void CallLogDetailsWindow::rebuild() {
	destroyInterface();
	buildInterface();
}

void CallLogDetailsWindow::decorateLabel(Label* label){
	if(label == nullptr){
		LOG_ERROR("label is nullptr");
		return;
	}
	label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
	label->setFont(style::window::font::small);
	label->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
	label->setLineMode(true); 
	// TODO: alek: try to use decorate() from Style.hpp
}
void CallLogDetailsWindow::decorateData(Label* label){
	if(label == nullptr){
		LOG_ERROR("label is nullptr");
		return;
	}
	label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
	label->setFont(style::window::font::big);
	label->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
	// TODO: alek: try to use decorate() from Style.hpp
}

void CallLogDetailsWindow::buildInterface() {
	AppWindow::buildInterface();

	bottomBar->setText( BottomBar::Side::LEFT, utils::localize.get("common_options"));
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_call"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	topBar->setActive(TopBar::Elements::TIME, true );

	// Information
	informationLabel = new gui::Label(this, leftMargin, 100, w()-leftMargin-rightMargin, 54, utils::localize.get("common_information"));
	decorateLabel(informationLabel);

	number = new gui::Label(this, leftMargin, 150, w()-leftMargin-rightMargin, 54);
	decorateData(number);
	number->setFont(style::window::font::bigbold); // overwrite default

	for( uint32_t i=0; i<2; ++i ) {
		rects[i] = new gui::Rect( this, 0,0, 50, 50 );
		rects[i]->setFilled( false );
		rects[i]->setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP );
		rects[i]->setPenFocusWidth(2);
		rects[i]->setPenWidth(0);
	}

	rects[static_cast<uint32_t>(FocusRects::Call)]->setPosition( 
		w()/2 + (w()-rightMargin)/2/3*1, 150 );
	rects[static_cast<uint32_t>(FocusRects::Sms)]->setPosition( 
		w()/2 + (w()-rightMargin)/2/3*2, 150 );

	//define navigation between labels
	rects[static_cast<uint32_t>(FocusRects::Call)]->setNavigationItem( NavigationDirection::LEFT,
		rects[static_cast<uint32_t>(FocusRects::Sms)]);
	rects[static_cast<uint32_t>(FocusRects::Call)]->setNavigationItem( NavigationDirection::RIGHT,
		rects[static_cast<uint32_t>(FocusRects::Sms)]);

	rects[static_cast<uint32_t>(FocusRects::Sms)]->setNavigationItem( NavigationDirection::LEFT,
		rects[static_cast<uint32_t>(FocusRects::Call)]);
	rects[static_cast<uint32_t>(FocusRects::Sms)]->setNavigationItem( NavigationDirection::RIGHT,
		rects[static_cast<uint32_t>(FocusRects::Call)]);

	//focus callbacks
	rects[static_cast<uint32_t>(FocusRects::Call)]->focusChangedCallback = [=] (gui::Item& item){
		LOG_INFO("CallImg gets focus" );
		bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_call"));
		return true; };

	rects[static_cast<uint32_t>(FocusRects::Sms)]->focusChangedCallback = [=] (gui::Item& item){
		LOG_INFO("SmsImg gets focus" );
		bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_send"));
		return true; };

	// Type
	typeLabel = new gui::Label(this, leftMargin, 200, w()/2-leftMargin, 54, utils::localize.get("app_calllog_type"));
	decorateLabel(typeLabel);

	const auto callTypeImgX = leftMargin +10;
	const auto callTypeImgY = 275+13;
	// TODO: alek: it is used in the code at least twice, possibly create one common function for this
	auto newImg = [=](const UTF8 imageName)->gui::Image* { 
		auto img = new gui::Image(this, callTypeImgX, callTypeImgY, 0, 0, imageName); 
		img->setVisible(false); 
		return img;  
	};
	callTypeImg[calllog::CallLogCallType::IN] = newImg("calllog_arrow_in");
	callTypeImg[calllog::CallLogCallType::OUT] = newImg("calllog_arrow_out");
	callTypeImg[calllog::CallLogCallType::MISSED] = newImg("calllog_arrow_den");

	auto typeDataX = callTypeImgX+ callTypeImg[0]->w() + 10;
	typeData = new gui::Label(this, typeDataX, 250, w()/2-typeDataX, 54);
	decorateData(typeData);

	// Duration
	const auto durationX = w()/2+leftMargin;
	const auto durationW = w()/2-leftMargin-rightMargin;
	durationLabel = new gui::Label(this, durationX, 200, durationW, 54, utils::localize.get("app_calllog_duration"));
	decorateLabel(durationLabel);

	durationData = new gui::Label(this, durationX, 250, durationW, 54);
	decorateData(durationData);	

	// Date
	dateLabel = new gui::Label(this, leftMargin, 300, w()-leftMargin-rightMargin, 54, utils::localize.get("app_calllog_date"));
	decorateLabel(dateLabel);

	dateDay = new gui::Label(this, leftMargin, 350, w()-leftMargin-rightMargin, 54);
	decorateData(dateDay);

	dateDate = new gui::Label(this, leftMargin, 400, w()-leftMargin-rightMargin, 54);
	decorateData(dateDate);
}

void CallLogDetailsWindow::destroyInterface() {
	AppWindow::destroyInterface();

	if( informationLabel ) { removeWidget(informationLabel); delete informationLabel; informationLabel = nullptr; }
	if( number ) { removeWidget(number); delete number; number = nullptr; }
	for( auto& rect : rects ) {
		if( rect ) { removeWidget(rect); delete rect; rect = nullptr;}
	}
	if( callImg ) { removeWidget(callImg); delete callImg; callImg = nullptr; }
	if( smsImg ) { removeWidget(smsImg); delete smsImg; smsImg = nullptr; }
	if( typeLabel ) { removeWidget(typeLabel); delete typeLabel; typeLabel = nullptr; }
	if( durationLabel ) { removeWidget(durationLabel); delete durationLabel; durationLabel = nullptr; }
	for( auto& img : callTypeImg ) {
		if( img ) { removeWidget(img); delete img; img = nullptr; }
	}
	if( typeData ) { removeWidget(typeData); delete typeData; typeData = nullptr; }
	if( durationData ) { removeWidget(durationData); delete durationData; durationData = nullptr; }
	if( dateLabel ) { removeWidget(dateLabel); delete dateLabel; dateLabel = nullptr; }
	if( dateDay ) { removeWidget(dateDay); delete dateDay; dateDay = nullptr; }	
	if( dateDate ) { removeWidget(dateDate); delete dateDate; dateDate = nullptr; }

	children.clear();
}

CallLogDetailsWindow::~CallLogDetailsWindow() {
	destroyInterface();
}

void CallLogDetailsWindow::onBeforeShow( ShowMode mode, SwitchData* data ) {
	if(data != nullptr && data->getDescription() == calllog::CALLLOG_SWITCH_DATA_STR) {
		auto switchData = reinterpret_cast<calllog::CallLogSwitchData*>(data);	
		record = switchData->getRecord();

		setTitle(record.name);
		
		number->setText(record.number.c_str());

		auto callType = toCallLogCallType( record.type );
		for (auto& img : callTypeImg) {
			img->setVisible(false);
		}
		callTypeImg[callType]->setVisible(true);
		
		UTF8 callTypeStr;
		switch(callType) {
			case CallLogCallType::IN: callTypeStr = utils::localize.get("app_calllog_incoming_call"); break;
			case CallLogCallType::OUT: callTypeStr = utils::localize.get("app_calllog_outgoing_call"); break;
			default: callTypeStr = utils::localize.get("app_calllog_missed_call"); break;
		}
		typeData->setText(callTypeStr);

		durationData->setText(utils::time::Time(record.duration).str("%Mm %Ss")); // TODO: alek: add duration class

		utils::time::Time t(record.date);
		dateDay->setText(t.day()+",");
		dateDate->setText(t.str(utils::localize.get("locale_date_full") + ", " + utils::localize.get("locale_12hour_min"))); // TODO: alek 12/24 h
	}

	if( mode == ShowMode::GUI_SHOW_INIT) setFocusItem( rects[static_cast<uint32_t>(FocusRects::Call)] );
}

bool CallLogDetailsWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	if( AppWindow::onInput( inputEvent ) ) {
		//refresh window only when key is other than enter
		if( inputEvent.keyCode != KeyCode::KEY_ENTER ) {
			application->render( RefreshModes::GUI_REFRESH_FAST );
		}

		return true;
	}

	//process only if key is released
	if(( inputEvent.state != InputEvent::State::keyReleasedShort ) &&
	   (( inputEvent.state != InputEvent::State::keyReleasedLong )))
		return false;

	switch( inputEvent.keyCode ) {
		case KeyCode::KEY_ENTER:
			LOG_INFO("Enter pressed");
			break;
		case KeyCode::KEY_LF: {
			std::unique_ptr<gui::SwitchData> data = std::make_unique<calllog::CallLogSwitchData>(record);
			application->switchWindow(calllog::settings::OptionsWindowStr, std::move(data));
			return true;
		}
		default:
			break;
	}

	return false;
}

} /* namespace gui */


