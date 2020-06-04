#include "ThreadViewWindow.hpp"

#include "../ApplicationMessages.hpp"
#include "../data/SMSdata.hpp"
#include "../models/ThreadModel.hpp"
#include "OptionsMessages.hpp"
#include "Span.hpp"

#include <Text.hpp>
#include <ListItem.hpp>
#include <ListItemProvider.hpp>
#include <ListView.hpp>
#include <Label.hpp>
#include <Margins.hpp>
#include <service-db/api/DBServiceAPI.hpp>
#include <service-appmgr/ApplicationManager.hpp>
#include <service-db/messages/DBMessage.hpp>
#include <service-cellular/api/CellularServiceAPI.hpp>
#include <application-phonebook/data/PhonebookItemData.hpp>
#include <i18/i18.hpp>
#include <time/time_conversion.hpp>
#include <log/log.hpp>
#include <Style.hpp>

#include <functional>
#include <memory>
#include <cassert>

namespace style
{}; // namespace style

namespace gui
{

    ThreadViewWindow::ThreadViewWindow(app::Application *app) : AppWindow(app, name::window::thread_view)
    {
        AppWindow::buildInterface();
        setTitle(utils::localize.get("app_messages_title_main"));
        topBar->setActive(TopBar::Elements::TIME, true);
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(style::strings::common::options));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        body = new gui::VBox(this,
                             style::window::default_left_margin,
                             title->offset_h(),
                             elements_width,
                             bottomBar->getY() - title->offset_h());
        body->setPenWidth(style::window::default_border_no_focus_w);
        body->setPenFocusWidth(style::window::default_border_no_focus_w);
        body->borderCallback = [this](const InputEvent &inputEvent) -> bool {
            if (inputEvent.state != InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (inputEvent.keyCode == KeyCode::KEY_UP) {
                return this->showMessages(Action::Next);
            }
            else if (inputEvent.keyCode == KeyCode::KEY_DOWN) {
                return this->showMessages(Action::Previous);
            }
            else {
                return false;
            }
        };

        rebuildText();
        /// setup
        body->setReverseOrder(true);
        body->setVisible(true);
        setFocusItem(body);
    }

    void ThreadViewWindow::rebuildText()
    {
        body->erase(text);
        text = new gui::Text(
            this, 0, 0, body->getWidth(), style::window::messages::sms_height, "", gui::Text::ExpandMode::EXPAND_UP);
        text->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &text) {},
            [=]() { textSelectSpecialCB(); }));
        text->setBorderColor(ColorNoColor);
        text->setPenFocusWidth(style::window::default_border_focucs_w);
        text->setPenWidth(style::window::default_border_focucs_w);
        text->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        text->activatedCallback = [&](gui::Item &item) {
            auto app = dynamic_cast<app::ApplicationMessages *>(application);
            assert(app != nullptr);
            if (app->handleSendSmsFromThread(contact->numbers[0].numberE164.c_str(), text->getText())) {
                LOG_ERROR("handleSendSmsFromThread failed");
            }
            return true;
        };
        text->inputCallback = [=](Item &, const InputEvent &event) {
            if (event.state == InputEvent::State::keyReleasedShort && event.keyCode == KeyCode::KEY_LF) {
                auto app = dynamic_cast<app::ApplicationMessages *>(application);
                assert(app != nullptr);
                return app->newMessageOptions(getName(), text);
            }
            return false;
        };
        text->focusChangedCallback = [=](Item &) -> bool {
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("sms_reply"));
            return true;
        };
    }

    void ThreadViewWindow::cleanView()
    {
        body->removeWidget(text);
        body->erase();
    }

    bool ThreadViewWindow::showMessages(ThreadViewWindow::Action what)
    {
        if (SMS.thread <= 0) {
            LOG_ERROR("threadID not set!");
            return false;
        }
        addSMS(what);
        return true;
    }

    void ThreadViewWindow::addSMS(ThreadViewWindow::Action what)
    {
        LOG_DEBUG("--- %d ---", static_cast<int>(what));
        // if there was text - then remove it temp
        gui::Alignment align;
        // 1. load elements to tmp vector
        SMS.dbsize = DBServiceAPI::SMSGetCount(this->application);
        LOG_DEBUG("start: %d end: %d db: %d", SMS.start, SMS.end, SMS.dbsize);
        if (what == Action::Start) {
            SMS.start = 0;
            SMS.end   = maxsmsinwindow;
            rebuildText();
        }

        // TODO 2. check how many of these will fit in box
        //         update begin / end in `SMS`
        if (what == Action::Next) {
            if (SMS.end != SMS.dbsize) {
                SMS.start = SMS.end;
            }
            else {
                LOG_INFO("All sms shown");
                return;
            }
        }
        else if (what == Action::Previous) {
            if (SMS.start == 0) {
                return;
            }
            else if (SMS.start - maxsmsinwindow < 0) {
                SMS.start = 0;
            }
            else {
                SMS.start -= maxsmsinwindow;
            }
            LOG_DEBUG("in progress %d", SMS.start);
        }
        SMS.sms = DBServiceAPI::SMSGetLimitOffsetByThreadID(this->application, SMS.start, maxsmsinwindow, SMS.thread);
        LOG_DEBUG("=> SMS %d < %d < %d",
                  static_cast<int>(SMS.start),
                  static_cast<int>(SMS.sms->size()),
                  static_cast<int>(maxsmsinwindow));
        // 3. add them to box
        this->cleanView();
        // if we are going from 0 then we want to show text prompt
        if (SMS.start == 0) {
            body->addWidget(text);
        }

        SMS.end = SMS.start;
        for (auto &el : *SMS.sms) {
            LOG_DEBUG("...");
            if (!smsBuild(el)) {
                break;
            }
            ++SMS.end;
        }
        body->setNavigation();
        setFocusItem(body);
        if (Action::Previous == what) {
            body->setVisible(true, true);
        }
        LOG_DEBUG("sms built");
    }

    HBox *ThreadViewWindow::smsSpanBuild(Text *smsBubble, const SMSRecord &el) const
    {
        HBox *labelSpan = new gui::HBox();

        labelSpan->setPenWidth(style::window::default_border_no_focus_w);
        labelSpan->setPenFocusWidth(style::window::default_border_no_focus_w);
        labelSpan->setSize(elements_width, smsBubble->getHeight());
        labelSpan->setMinimumWidth(elements_width);
        labelSpan->setMinimumHeight(smsBubble->getHeight());
        labelSpan->setFillColor(gui::Color(11, 0));

        LOG_DEBUG("ADD SMS TYPE: %d", static_cast<int>(el.type));
        switch (el.type) {
        case SMSType::QUEUED:
            // Handle in the same way as case below. (pending sending display as already sent)
        case SMSType::OUTBOX:
            smsBubble->setYaps(RectangleYapFlags::GUI_RECT_YAP_TOP_RIGHT);
            smsBubble->setX(body->getWidth() - smsBubble->getWidth());
            labelSpan->setReverseOrder(true);
            labelSpan->addWidget(smsBubble);
            addTimeLabel(
                labelSpan, timeLabelBuild(el.date), elements_width - (smsBubble->getWidth() + smsBubble->yapSize));
            break;
        case SMSType::INBOX:
            smsBubble->setYaps(RectangleYapFlags::GUI_RECT_YAP_TOP_LEFT);
            labelSpan->setReverseOrder(false);
            labelSpan->addWidget(smsBubble);
            addTimeLabel(
                labelSpan, timeLabelBuild(el.date), elements_width - (smsBubble->getWidth() + smsBubble->yapSize));
            break;
        case SMSType::FAILED:
            smsBubble->setYaps(RectangleYapFlags::GUI_RECT_YAP_TOP_RIGHT);
            smsBubble->setX(body->getWidth() - smsBubble->getWidth());
            labelSpan->setReverseOrder(true);
            addErrorLabel(labelSpan, elements_width - (smsBubble->getWidth() + smsBubble->yapSize));
            labelSpan->addWidget(smsBubble);
        default:
            break;
        }

        if (!smsBubble->visible) {
            delete labelSpan; // total fail
            labelSpan = nullptr;
        }
        return labelSpan;
    }

    void ThreadViewWindow::addErrorLabel(HBox *layout, uint16_t widthAvailable) const
    {
        auto errorIcon = new gui::Image("messages_error_W_M");
        // set icon vertical aligmnet to center
        errorIcon->setY((layout->getHeight() - errorIcon->getHeight()) / 2);
        errorIcon->activeItem = false; // make it non-focusable

        layout->addWidget(new Span(Axis::X, style::window::messages::sms_error_icon_offset));
        layout->addWidget(errorIcon);
        layout->addWidget(new Span(Axis::X,
                                   style::window::messages::sms_failed_offset -
                                       (errorIcon->getWidth() + style::window::messages::sms_error_icon_offset)));
    }

    void ThreadViewWindow::addTimeLabel(HBox *layout, Label *timeLabel, uint16_t widthAvailable) const
    {
        // add time label activated on focus
        timeLabel->setSize(timeLabel->getWidth(), layout->getHeight());
        timeLabel->setSize(timeLabel->getTextNeedSpace(), timeLabel->getHeight());
        timeLabel->setMinimumWidth(timeLabel->getWidth());
        timeLabel->setMinimumHeight(timeLabel->getHeight());

        uint16_t timeLabelSpacerWidth = widthAvailable - timeLabel->getWidth();

        layout->addWidget(new gui::Span(Axis::X, timeLabelSpacerWidth));
        layout->addWidget(timeLabel);

        layout->focusChangedCallback = [=](gui::Item &item) {
            timeLabel->setVisible(item.focus);
            // we need to inform parent that it needs to resize itself - easiest way to do so
            if (timeLabel->parent) {
                timeLabel->parent->setSize(timeLabel->parent->getWidth(), timeLabel->parent->getHeight());
            }
            return true;
        };
    }

    Label *ThreadViewWindow::timeLabelBuild(time_t timestamp) const
    {
        auto timeLabel        = new gui::Label(nullptr, 0, 0, 0, 0);
        timeLabel->activeItem = false;
        timeLabel->setFont(style::window::font::verysmall);
        timeLabel->setText(utils::time::Time(timestamp));
        timeLabel->setPenWidth(style::window::default_border_no_focus_w);
        timeLabel->setVisible(false);
        timeLabel->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
        return timeLabel;
    }

    bool ThreadViewWindow::smsBuild(const SMSRecord &smsRecord)
    {
        /// dummy sms thread - TODO load from db - on switchData
        auto smsLabel = new Text(nullptr, 0, 0, style::window::messages::sms_max_width, 0);
        smsLabel->setTextType(Text::TextType::MULTI_LINE);
        smsLabel->setEditMode(Text::EditMode::SCROLL);
        smsLabel->setEdges(RectangleEdgeFlags::GUI_RECT_ALL_EDGES);
        smsLabel->setRadius(style::window::messages::sms_radius);
        smsLabel->setFont(style::window::font::medium);
        smsLabel->setPenFocusWidth(style::window::default_border_focucs_w);
        smsLabel->setPenWidth(style::window::messages::sms_border_no_focus);
        smsLabel->expandMode = Text::ExpandMode::EXPAND_DOWN;
        smsLabel->setText(smsRecord.body.length() ? smsRecord.body : " "); // text doesn't really like being empty//
        smsLabel->setMargins(gui::Margins(style::window::messages::sms_h_padding,
                                          style::window::messages::sms_v_padding,
                                          style::window::messages::sms_h_padding,
                                          style::window::messages::sms_v_padding));

        smsLabel->inputCallback = [=, &smsRecord](Item &, const InputEvent &event) {
            if (event.state == InputEvent::State::keyReleasedShort && event.keyCode == KeyCode::KEY_LF) {
                LOG_INFO("Message activated!");
                auto app = dynamic_cast<app::ApplicationMessages *>(application);
                assert(app != nullptr);
                if (app->windowOptions != nullptr) {
                    app->windowOptions->clearOptions();
                    app->windowOptions->addOptions(smsWindowOptions(app, smsRecord));
                    app->switchWindow(app->windowOptions->getName(), nullptr);
                }
                return true;
            }
            return false;
        };
        smsLabel->focusChangedCallback = [=](gui::Item &item) {
            bottomBar->setActive(BottomBar::Side::CENTER, false);
            return true;
        };

        // wrap label in H box, to make fit datetime in it
        HBox *labelSpan = smsSpanBuild(smsLabel, smsRecord);

        if (labelSpan == nullptr) {
            return false;
        }

        body->addWidget(new gui::Span(Axis::Y, style::window::messages::sms_vertical_spacer));

        LOG_INFO("Add sms: %s %s", smsRecord.body.c_str(), smsRecord.number.c_str());
        body->addWidget(labelSpan);
        return labelSpan->visible;
    }

    void ThreadViewWindow::rebuild()
    {
        addSMS(ThreadViewWindow::Action::Start);
    }

    void ThreadViewWindow::buildInterface()
    {}

    void ThreadViewWindow::destroyInterface()
    {
        erase();
    }

    ThreadViewWindow::~ThreadViewWindow()
    {
        destroyInterface();
    }

    void ThreadViewWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        {
            auto pdata = dynamic_cast<SMSThreadData *>(data);
            if (pdata) {
                LOG_INFO("We have it! %" PRIu32, pdata->thread->dbID);
                cleanView();
                SMS.thread = pdata->thread->dbID;
                showMessages(Action::Start);
                auto ret = DBServiceAPI::ContactGetByID(application, pdata->thread->contactID);
                contact  = std::make_shared<ContactRecord>(ret->front());
                // should be name number for now - easier to handle
                setTitle(ret->front().getFormattedName());
                return;
            }
        }
        if (auto pdata = dynamic_cast<SMSTextData *>(data)) {
            auto txt = pdata->text;
            LOG_INFO("received sms templates data \"%s\"", txt.c_str());
            text->setText(text->getText() + txt);
        }
    }

    bool ThreadViewWindow::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }

    bool ThreadViewWindow::onDatabaseMessage(sys::Message *msgl)
    {
        //	DBContactResponseMessage* msg = reinterpret_cast<DBContactResponseMessage*>( msgl );
        //	if( phonebookModel->updateRecords( std::move(msg->records), msg->offset, msg->limit, msg->count,
        // msg->favourite ) ) 		return true;

        return false;
    }

} /* namespace gui */
