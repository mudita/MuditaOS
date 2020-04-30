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

#include "../data/CallLogInternals.hpp" // TODO: alek: add easier paths
#include "../data/CallLogSwitchData.hpp"
#include "../windows/CallLogOptionsWindow.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "UiCommonActions.hpp"
#include "application-call/ApplicationCall.hpp"
#include "time/time_conversion.hpp"
#include <Style.hpp>
#include <cassert>

using namespace calllog;
using namespace callLogStyle::detailsWindow;

namespace gui
{

    CallLogDetailsWindow::CallLogDetailsWindow(app::Application *app)
        : AppWindow(app, calllog::settings::DetailsWindowStr)
    {

        buildInterface();
    }

    void CallLogDetailsWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    Label *CallLogDetailsWindow::decorateLabel(Label *label)
    {
        if (label == nullptr) {
            LOG_ERROR("label is nullptr");
            return label;
        }
        style::window::decorate(label);
        label->setFont(style::window::font::small);
        label->setSize(label->widgetArea.w, style::window::label::big_h);
        label->setLineMode(true);

        return label;
    }

    Label *CallLogDetailsWindow::decorateData(Label *label)
    {
        if (label == nullptr) {
            LOG_ERROR("label is nullptr");
            return label;
        }
        style::window::decorate(label);
        label->setFont(style::window::font::medium);
        label->setSize(label->widgetArea.w, style::window::label::small_h);

        return label;
    }

    void CallLogDetailsWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(style::strings::common::options));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::call));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        topBar->setActive(TopBar::Elements::TIME, true);

        // NOTE: height of all labels is set using decorators

        // Information
        informationLabel = decorateLabel(new gui::Label(this,
                                                        information::label::x,
                                                        information::label::y,
                                                        information::label::w,
                                                        0,
                                                        utils::localize.get(style::strings::common::information)));
        number           = decorateData(
            new gui::Label(this, information::number::x, information::number::y, information::number::w, 0));
        number->setFont(style::window::font::mediumbold);

        for (uint32_t i = 0; i < 2; ++i) {
            rects[i] = new gui::Rect(this, 0, 0, information::imgs::w, information::imgs::h);
            rects[i]->setFilled(false);
            rects[i]->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
            rects[i]->setPenFocusWidth(style::window::default_border_focucs_w);
            rects[i]->setPenWidth(style::window::default_border_no_focus_w);
        }

        rects[static_cast<uint32_t>(FocusRects::Call)]->setPosition(information::imgs::call::x, information::imgs::y);
        rects[static_cast<uint32_t>(FocusRects::Sms)]->setPosition(information::imgs::sms::x, information::imgs::y);

        // TODO: alek: phone ringing seems to be to small
        callImg = new gui::Image(rects[FocusRects::Call],
                                 information::imgs::call::icon::x,
                                 information::imgs::call::icon::y,
                                 0,
                                 0,
                                 "phonebook_phone_ringing");
        smsImg  = new gui::Image(
            rects[FocusRects::Sms], information::imgs::sms::icon::x, information::imgs::call::icon::y, 0, 0, "mail");

        // define navigation between labels
        rects[static_cast<uint32_t>(FocusRects::Call)]->setNavigationItem(
            NavigationDirection::LEFT, rects[static_cast<uint32_t>(FocusRects::Sms)]);
        rects[static_cast<uint32_t>(FocusRects::Call)]->setNavigationItem(
            NavigationDirection::RIGHT, rects[static_cast<uint32_t>(FocusRects::Sms)]);

        rects[static_cast<uint32_t>(FocusRects::Sms)]->setNavigationItem(
            NavigationDirection::LEFT, rects[static_cast<uint32_t>(FocusRects::Call)]);
        rects[static_cast<uint32_t>(FocusRects::Sms)]->setNavigationItem(
            NavigationDirection::RIGHT, rects[static_cast<uint32_t>(FocusRects::Call)]);

        // focus callbacks
        rects[static_cast<uint32_t>(FocusRects::Call)]->focusChangedCallback = [=](gui::Item &item) {
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::call));
            return true;
        };

        rects[static_cast<uint32_t>(FocusRects::Sms)]->focusChangedCallback = [=](gui::Item &item) {
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::send));
            return true;
        };

        // activated callbacks
        rects[FocusRects::Call]->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("call %s", record.number.c_str());
            return app::call(application, app::CallOperation::ExecuteCall, record.number);
        };

        rects[FocusRects::Sms]->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("sms %s", record.number.c_str());
            return app::sms(application, app::SmsOperation::Add, record.number);
        };

        // Type
        typeLabel = decorateLabel(new gui::Label(
            this, type::label::x, type::label::y, type::label::w, 0, utils::localize.get("app_calllog_type")));
        typeData  = decorateData(new gui::Label(this, type::data::x, type::data::y, type::data::w, 0));

        // TODO: alek: it is used in the code at least twice, possibly create one common function for this
        auto newImg = [=](const UTF8 imageName) -> gui::Image * {
            auto img = new gui::Image(this, type::img::x, type::img::y, 0, 0, imageName);
            img->setVisible(false);
            return img;
        };
        callTypeImg[calllog::CallLogCallType::IN]     = newImg("calllog_arrow_in");
        callTypeImg[calllog::CallLogCallType::OUT]    = newImg("calllog_arrow_out");
        callTypeImg[calllog::CallLogCallType::MISSED] = newImg("calllog_arrow_den");

        // Duration
        durationLabel = decorateLabel(new gui::Label(this,
                                                     duration::label::x,
                                                     duration::label::y,
                                                     duration::label::w,
                                                     0,
                                                     utils::localize.get("app_calllog_duration")));
        durationData  = decorateData(new gui::Label(this, duration::data::x, duration::data::y, duration::data::w, 0));

        // Date
        dateLabel = decorateLabel(new gui::Label(
            this, date::label::x, date::label::y, date::label::w, 0, utils::localize.get("app_calllog_date")));
        dateDay   = decorateData(new gui::Label(this, date::dataDay::x, date::dataDay::y, date::dataDay::w, 0));
        dateDate  = decorateData(new gui::Label(this, date::dataDate::x, date::dataDate::y, date::dataDate::w, 0));
    }

    void CallLogDetailsWindow::destroyInterface()
    {
        erase();
    }

    CallLogDetailsWindow::~CallLogDetailsWindow()
    {
        destroyInterface();
    }

    void CallLogDetailsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (data != nullptr && data->getDescription() == calllog::CALLLOG_SWITCH_DATA_STR) {
            auto switchData = reinterpret_cast<calllog::CallLogSwitchData *>(data);
            record          = switchData->getRecord();

            setTitle(record.name);

            number->setText(record.number.c_str());

            auto callType = toCallLogCallType(record.type);
            for (auto &img : callTypeImg) {
                img->setVisible(false);
            }
            callTypeImg[callType]->setVisible(true);

            UTF8 callTypeStr;
            switch (record.type) {
            case CallType::CT_INCOMING:
                callTypeStr = utils::localize.get("app_calllog_incoming_call");
                break;
            case CallType::CT_OUTGOING:
                callTypeStr = utils::localize.get("app_calllog_outgoing_call");
                break;
            case CallType::CT_MISSED:
                callTypeStr = utils::localize.get("app_calllog_missed_call");
                break;
            case CallType::CT_REJECTED:
                callTypeStr = utils::localize.get("app_calllog_rejected_call");
                break;
            default:
                break;
            }
            typeData->setText(callTypeStr);

            durationData->setText(utils::time::Duration(record.duration).str());

            utils::time::Timestamp t(record.date);
            dateDay->setText(t.day() + ",");
            dateDate->setText(t.str(utils::localize.get("locale_date_full") + ", " +
                                    utils::localize.get("locale_12hour_min"))); // TODO: alek 12/24 h
        }

        if (mode == ShowMode::GUI_SHOW_INIT)
            setFocusItem(rects[static_cast<uint32_t>(FocusRects::Call)]);
    }

    bool CallLogDetailsWindow::onInput(const InputEvent &inputEvent)
    {
        // check if any of the lower inheritance onInput methods catch the event
        if (AppWindow::onInput(inputEvent)) {
            // refresh window only when key is other than enter
            if (inputEvent.keyCode != KeyCode::KEY_ENTER) {
                application->render(RefreshModes::GUI_REFRESH_FAST);
            }

            return true;
        }

        // process only if key is released
        if (((inputEvent.state == InputEvent::State::keyReleasedShort) ||
             ((inputEvent.state == InputEvent::State::keyReleasedLong))) &&
            (inputEvent.keyCode == KeyCode::KEY_LF)) {
            auto app = dynamic_cast<app::ApplicationCallLog *>(application);
            assert(app != nullptr);

            if (app->windowOptions != nullptr) {
                app->windowOptions->clearOptions();
                app->windowOptions->addOptions(calllogWindowOptions(app, record));
                app->switchWindow(app->windowOptions->getName(), nullptr);
            }

            return true;
        }

        return false;
    }

} /* namespace gui */
