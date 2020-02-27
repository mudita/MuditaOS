/*
 * @file DesktopMainWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 19 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <memory>
#include <functional>

#include "../ApplicationDesktop.hpp"
#include "../data/LockPhoneData.hpp"
#include "Alignment.hpp"
#include "Common.hpp"
#include "DesktopMainWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "service-appmgr/ApplicationManager.hpp"

//application-call
#include "application-call/ApplicationCall.hpp"
#include "application-call/data/CallSwitchData.hpp"

#include "i18/i18.hpp"
#include <Span.hpp>
#include <Style.hpp>
#include <time/time_conversion.hpp>

#include "service-db/api/DBServiceAPI.hpp"

namespace style
{
    const auto design_time_offset = 106;
    const auto design_time_h = 96;
    const auto design_day_offset = 204;
    const auto design_day_h = 51;
    const auto design_border_offset = 20;
    const auto design_option_span = 8;
    const auto design_notifications_offset = 284;
}; // namespace style

namespace gui {

    void DesktopMainWindow::buildInterface()
    {

        auto ttime = utils::time::Time();
        AppWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_desktop_unlock"));
        topBar->setActive({{TopBar::Elements::SIGNAL, true}, {TopBar::Elements::LOCK, true}, {TopBar::Elements::BATTERY, true}});

        time = new gui::Label(this, 0, style::design_time_offset, style::window_width, style::design_time_h);
        time->setFilled(false);
        time->setBorderColor(gui::ColorNoColor);
        time->setFont(style::window::font::supersizeme);
        time->setText(ttime);
        time->setAlignement(Alignment::ALIGN_HORIZONTAL_CENTER);

        dayText = new gui::Label(this, 0, style::design_day_offset, style::window_width, style::design_day_h);
        dayText->setFilled(false);
        dayText->setBorderColor(gui::ColorNoColor);
        dayText->setFont(style::window::font::biglight);
        dayText->setText(ttime.day() + ", " + ttime.str("%d %b"));
        dayText->setAlignement(Alignment::ALIGN_HORIZONTAL_CENTER);

}

void DesktopMainWindow::destroyInterface() {
	AppWindow::destroyInterface();
	delete time;
	delete dayText;
	focusItem = nullptr;
	children.clear();
}

DesktopMainWindow::DesktopMainWindow(app::Application *app) : AppWindow(app, gui::name::window::main_window)
{
	buildInterface();
}

DesktopMainWindow::~DesktopMainWindow() {
	destroyInterface();
}

void DesktopMainWindow::setVisibleState() {

    auto app = dynamic_cast<app::ApplicationDesktop *>(application);
    if (!app)
    {
        return;
    }

    if( app->getScreenLocked() ) {
		bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("app_desktop_unlock"));
		topBar->setActive( TopBar::Elements::LOCK, true );
	}
	else {
		bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("app_desktop_menu"));
		topBar->setActive( TopBar::Elements::LOCK, false );
	}
    if (!fillNotifications())
    {
        LOG_ERROR("Couldn't fit in all notifications");
    }
}

void DesktopMainWindow::onBeforeShow( ShowMode mode, SwitchData* data ) {

	//update time
	time->setText( topBar->getTimeString() );

	//check if there was a signal to lock the pone due to inactivity.
	if( (data != nullptr) && (data->getDescription() == "LockPhoneData")) {
        auto app = dynamic_cast<app::ApplicationDesktop *>(application);
        if (!app)
        {
            return;
        }
        app->setScreenLocked(true);

		LockPhoneData* lockData = reinterpret_cast<LockPhoneData*>( data );
		lockTimeoutApplilcation = lockData->getPreviousApplication();

		reinterpret_cast<app::ApplicationDesktop*>(application)->setSuspendFlag(true);
	}

	setVisibleState();
}

bool DesktopMainWindow::onInput( const InputEvent& inputEvent ) {
	
	// do nothing
	if( inputEvent.state == InputEvent::State::keyReleasedShort && inputEvent.keyCode == KeyCode::KEY_RF ) return false;
	
	app::ApplicationDesktop* app = reinterpret_cast<app::ApplicationDesktop*>( application );

	//process shortpress
	if( inputEvent.state == InputEvent::State::keyReleasedShort ) {

		if( app->getScreenLocked() ) {
			//if enter was pressed
			if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
				unlockStartTime = xTaskGetTickCount();
				enterPressed = true;
			}
			else if(( inputEvent.keyCode == KeyCode::KEY_PND ) && enterPressed ) {
				//if interval between enter and pnd keys is less than time defined for unlocking
				if( xTaskGetTickCount() - unlockStartTime  < unclockTime) {
					//display pin lock screen or simply refresh current window to update labels
					if( app->getPinLocked())
						//if there was no application on to before closing proceed normally to pin protection window.
						if( lockTimeoutApplilcation.empty()) {
							application->switchWindow( "PinLockWindow" );
						}
						else {
							std::unique_ptr<LockPhoneData> data = std::make_unique<LockPhoneData>();
							data->setPrevApplication( lockTimeoutApplilcation );
							lockTimeoutApplilcation = "";
							application->switchWindow( "PinLockWindow", std::move(data) );
						}

					else {

						//if phone was locked by user show unlocked main window
						if( lockTimeoutApplilcation.empty() ) {
							app->setScreenLocked(false);
							setVisibleState();
							application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
						}
						//if there was application on top when timeout occurred
						else {
							lockTimeoutApplilcation = "";
							sapm::ApplicationManager::messageSwitchPreviousApplication( application );
						}
					}
				}
				enterPressed = false;
			}
			else {
				enterPressed = false;
			}
		}
		//screen is unlocked
		else {
            int val = gui::toNumeric(inputEvent.keyCode);
			//pressing enter moves user to menu screen
			if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
				application->switchWindow( "MenuWindow" );
			}
			//if numeric key was pressed record that key and send it to call application with a switch command
            else if( 0 <= val && val <= 9)
            {
                auto key = std::to_string(val);
                std::unique_ptr<gui::SwitchData> phoneNumberData = std::make_unique<app::EnterNumberData>(key);
                sapm::ApplicationManager::messageSwitchApplication(application, app::name_call, "EnterNumberWindow", std::move(phoneNumberData));
                return true;
            }
		}
	}
	else if( inputEvent.state == InputEvent::State::keyReleasedLong ) {
		//long press of # locks screen if it was unlocked
		if( (inputEvent.keyCode == KeyCode::KEY_PND) && ( app->getScreenLocked() == false ) ) {
			app::ApplicationDesktop* app = reinterpret_cast<app::ApplicationDesktop*>( application );
			app->setScreenLocked(true);
			setVisibleState();
			application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

			reinterpret_cast<app::ApplicationDesktop*>(application)->setSuspendFlag(true);
		}
		//long press of right function button move user to power off window
		else if (inputEvent.keyCode == KeyCode::KEY_RF) {
			application->switchWindow( "PowerOffWindow" );
            return true;
		}
	}

	//check if any of the lower inheritance onInput methods catch the event
	return AppWindow::onInput( inputEvent );
}

void DesktopMainWindow::rebuild() {
	destroyInterface();
	buildInterface();
}

bool DesktopMainWindow::updateTime( const UTF8& timeStr ) {
	auto ret = AppWindow::updateTime( timeStr );
	time->setText( topBar->getTimeString());
	return ret;
}
bool DesktopMainWindow::updateTime( const uint32_t& timestamp, bool mode24H ) {
	auto ret = AppWindow::updateTime( timestamp, mode24H );
	time->setText( topBar->getTimeString());
	return ret;
}

std::list<DrawCommand*> DesktopMainWindow::buildDrawList() {
	time->setText( topBar->getTimeString());
    return gui::AppWindow::buildDrawList();
}

/// try item to parent, if fails -> remove item
auto try_add_del(Item *parent, Item *item)
{
    if (parent->addWidget(item))
    {
        return true;
    }
    delete item;
    return false;
}

auto add_box_icon(gui::BoxLayout *layout, UTF8 icon)
{
    auto thumbnail = new gui::Image(icon);
    auto center = (layout->h() - thumbnail->h()) / 2;
    if (center > 0)
    {
        thumbnail->widgetArea.pos(Axis::Y) += center;
    }
    thumbnail->activeItem = false;
    return try_add_del(layout, thumbnail);
}

/// for now notifications are like that: `^<span>[icon]<span>[dumb text]       [dot image] [number of notifications]<span>$`
auto add_notification(BoxLayout *layout, UTF8 icon, UTF8 name, UTF8 indicator, std::function<bool(Item &)> foo) -> bool
{
    const auto text_normal_size = 200;
    const auto size_needed_for_2digits = 30;
    auto el = new gui::HBox(nullptr, 0, 0, layout->getWidth(), style::window::label::default_h);
    do
    {
        /// box left inner margin
        auto span = new gui::Span(Axis::X, style::design_border_offset);
        if (!try_add_del(el, span))
        {
            break;
        }
        if (!add_box_icon(el, icon))
        {
            break;
        }
        auto span2 = new gui::Span(Axis::X, style::design_border_offset);
        if (!try_add_del(el, span2))
        {
            break;
        }
        auto text = new gui::Label(nullptr, 0, 0, text_normal_size, style::window::label::default_h, "");
        {
            // set area from 0 to normal
            text->area(Item::Area::Max) = text->area(Item::Area::Normal);
            // set max area for text in axis to max
            text->area(Item::Area::Max).size(Axis::X) = el->w();
        }
        text->setText(name);
        text->setFont(style::window::font::medium);
        text->setAlignement(Alignment::ALIGN_VERTICAL_CENTER);
        text->setPenWidth(0);
        text->activeItem = false;
        if (!try_add_del(el, text))
        {
            break;
        }
        if (!add_box_icon(el, "dot_12px_soft"))
        {
            break;
        }
        auto number = new gui::Label();
        number->setText(indicator);
        number->setFont(style::window::font::mediumbold);
        number->setPenWidth(style::window::default_border_no_focus_w);
        number->setSize(size_needed_for_2digits, el->h());
        number->setAlignement(Alignment::ALIGN_VERTICAL_CENTER | Alignment::ALIGN_HORIZONTAL_RIGHT);
        number->activeItem = false;
        if (!try_add_del(el, number))
        {
            break;
        }
        /// box right inner margin
        auto span3 = new gui::Span(Axis::X, style::design_border_offset);
        if (!try_add_del(el, span3))
        {
            break;
        }

        el->setPenWidth(style::window::default_border_no_focus_w);
        el->setPenFocusWidth(style::window::default_border_focucs_w);
        el->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
        if (!try_add_del(layout, el))
        {
            el = nullptr;
            break;
        }
        /// space between next notifications to show
        layout->addWidget(new gui::Span(Axis::Y, style::design_option_span));
        return true;
    } while (false);
    delete el;
    return false;
}

auto DesktopMainWindow::fillNotifications() -> bool
{
    auto app = dynamic_cast<app::ApplicationDesktop *>(application);
    if (!app || app->getScreenLocked())
    {
        return false;
    }
    if (notifications)
    {
        this->removeWidget(notifications);
        delete notifications;
    }
    // 1. create notifications box
    notifications =
        new gui::VBox(nullptr, style::window::default_left_margin, style::design_notifications_offset,
                      style::window_width - 2 * style::window::default_left_margin, bottomBar->widgetArea.pos(Axis::Y) - style::design_notifications_offset);
    notifications->setPenWidth(style::window::default_border_no_focus_w);
    notifications->setPenFocusWidth(style::window::default_border_no_focus_w);
    if (!this->addWidget(notifications))
    {
        LOG_ERROR("Can't create notifications box!");
        delete notifications;
        return false;
    }

    // 2. actually fill it in
    auto unhandled_calls = DBServiceAPI::CalllogGetCount(application, CallState::MISSED);
    if (unhandled_calls)
    {
        add_notification(notifications, "phone", utils::localize.get("app_desktop_missed_calls"), std::to_string(unhandled_calls), [](Item &) { return true; });
    }
    auto unread_sms = DBServiceAPI::SMSGetCount(this->application, SMSState::UNREAD);
    if (unread_sms)
    {
        add_notification(notifications, "mail", utils::localize.get("app_desktop_unread_messages"), std::to_string(unread_sms), [](Item &) { return true; });
    }
    setFocusItem(notifications);
    return true;
}

} /* namespace gui */
