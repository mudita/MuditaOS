#include "ThreadViewWindow.hpp"

#include "../ApplicationMessages.hpp"
#include "../data/SMSdata.hpp"
#include "../models/ThreadModel.hpp"
#include "OptionsMessages.hpp"
#include "Service/Message.hpp"
#include "Span.hpp"

#include <Text.hpp>
#include <TextBubble.hpp>
#include <ListItem.hpp>
#include <ListItemProvider.hpp>
#include <ListView.hpp>
#include <Label.hpp>
#include <Margins.hpp>
#include <service-db/api/DBServiceAPI.hpp>
#include <service-appmgr/ApplicationManager.hpp>
#include <service-db/messages/DBMessage.hpp>
#include <service-db/messages/DBNotificationMessage.hpp>
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
                return this->showMessages(Action::NextPage);
            }
            else if (inputEvent.keyCode == KeyCode::KEY_DOWN) {
                return this->showMessages(Action::PreviousPage);
            }
            else {
                return false;
            }
        };

        refreshTextItem();
        /// setup
        body->setReverseOrder(true);
        body->setVisible(true);
        setFocusItem(body);
    }

    void ThreadViewWindow::refreshTextItem()
    {
        if (text != nullptr) {
            return;
        }
        text = new gui::Text(nullptr, 0, 0, 0, 0, "", ExpandMode::EXPAND_UP);
        text->setMinimumSize(body->getWidth(), text->getHeight());
        text->setMaximumSize(body->getWidth(), body->getHeight());
        text->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &text) { bottomBarTemporaryMode(text); },
            [=]() { bottomBarRestoreFromTemporaryMode(); },
            [=]() { selectSpecialCharacter(); }));
        text->setBorderColor(ColorNoColor);
        text->setPenFocusWidth(style::window::default_border_focus_w);
        text->setPenWidth(style::window::default_border_focus_w);
        text->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        text->activatedCallback = [&](gui::Item &item) {
            auto app = dynamic_cast<app::ApplicationMessages *>(application);
            assert(app != nullptr);
            if (app->handleSendSmsFromThread(contact->numbers[0].number, text->getText())) {
                LOG_ERROR("handleSendSmsFromThread failed");
            }
            text->clear();
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
        text->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    }

    void ThreadViewWindow::destroyTextItem()
    {
        body->erase(text);
        text = nullptr;
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
        std::unique_ptr<ThreadRecord> threadDetails = DBServiceAPI::ThreadGet(this->application, SMS.thread);
        if (threadDetails == nullptr) {
            LOG_ERROR("cannot fetch details of selected thread (id: %d)", SMS.thread);
            return;
        }
        SMS.dbsize = threadDetails->msgCount;

        if (threadDetails != nullptr && threadDetails->isUnread()) {
            auto app = dynamic_cast<app::ApplicationMessages *>(application);
            assert(app != nullptr);
            app->markSmsThreadAsRead(threadDetails->ID);
        }

        LOG_DEBUG("start: %d end: %d db: %d", SMS.start, SMS.end, SMS.dbsize);
        if (what == Action::Init || what == Action::NewestPage) {
            if (what == Action::NewestPage) {
                SMS.start = 0;
                SMS.end   = maxsmsinwindow;
            }
            if (what == Action::Init) {
                destroyTextItem();
            }
            refreshTextItem();
        }

        // 2. check how many of these will fit in box
        //         update begin / end in `SMS`
        if (what == Action::NextPage) {
            if (SMS.end != SMS.dbsize) {
                SMS.start = SMS.end;
            }
            else {
                LOG_INFO("All sms shown");
                return;
            }
        }
        else if (what == Action::PreviousPage) {
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
        if (SMS.sms->size() == 0) {
            LOG_WARN("Deleting bad thread. There are no messages belonging to it (id: %d)", SMS.thread);
            DBServiceAPI::ThreadRemove(this->application, SMS.thread);
            return;
        }
        // 3. add them to box
        this->cleanView();
        // if we are going from 0 then we want to show text prompt
        if (SMS.start == 0) {
            body->addWidget(text);
            // to set proper text size required for element, not max possible size
            text->setText(text->getText());
        }

        // rebuild bubbles
        SMS.end = SMS.start;
        for (auto &el : *SMS.sms) {
            if (!smsBuild(el)) {
                break;
            }
            ++SMS.end;
        }
        body->setNavigation();
        setFocusItem(body);
        if (Action::PreviousPage == what) {
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
        auto max_available_h = body->area().h;
        auto max_available_w = style::window::messages::sms_max_width;
        /// dummy sms thread - TODO load from db - on switchData
        auto smsBubble = new TextBubble(nullptr, 0, 0, style::window::messages::sms_max_width, 0);
        smsBubble->setMaximumSize(max_available_w, max_available_h);
        smsBubble->setTextType(TextType::MULTI_LINE);
        smsBubble->setRadius(style::window::messages::sms_radius);
        smsBubble->setFont(style::window::font::medium);
        smsBubble->setPenFocusWidth(style::window::default_border_focus_w);
        smsBubble->setPenWidth(style::window::messages::sms_border_no_focus);
        smsBubble->setMargins(gui::Margins(style::window::messages::sms_h_padding,
                                           style::window::messages::sms_v_padding,
                                           style::window::messages::sms_h_padding,
                                           style::window::messages::sms_v_padding));
        smsBubble->setText(smsRecord.body);

        smsBubble->inputCallback = [=, &smsRecord](Item &, const InputEvent &event) {
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
        smsBubble->focusChangedCallback = [=](gui::Item &item) {
            bottomBar->setActive(BottomBar::Side::CENTER, false);
            return true;
        };

        // wrap label in H box, to make fit datetime in it
        HBox *labelSpan = smsSpanBuild(smsBubble, smsRecord);

        if (labelSpan == nullptr) {
            return false;
        }

        body->addWidget(new gui::Span(Axis::Y, style::window::messages::sms_vertical_spacer));

        LOG_INFO("Add sms: %s %s", smsRecord.body.c_str(), smsRecord.number.getFormatted().c_str());
        body->addWidget(labelSpan);
        return labelSpan->visible;
    }

    void ThreadViewWindow::rebuild()
    {
        addSMS(ThreadViewWindow::Action::Init);
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
                LOG_INFO("We have it! %" PRIu32, pdata->thread->ID);
                cleanView();
                SMS.thread = pdata->thread->ID;
                showMessages(Action::Init);
                auto ret = DBServiceAPI::ContactGetByID(application, pdata->thread->contactID);
                contact  = std::make_shared<ContactRecord>(ret->front());
                // should be name number for now - easier to handle
                setTitle(ret->front().getFormattedName());
            }
        }
        if (auto pdata = dynamic_cast<SMSTextData *>(data)) {
            auto txt = pdata->text;
            LOG_INFO("received sms templates data \"%s\"", txt.c_str());
            text->addText(txt);
        }
    }

    bool ThreadViewWindow::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }

    bool ThreadViewWindow::onDatabaseMessage(sys::Message *msgl)
    {
        auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msg != nullptr) {
            if (msg->interface == db::Interface::Name::SMS) {
                std::unique_ptr<ThreadRecord> threadDetails;
                switch (msg->type) {
                case db::Query::Type::Create:
                    // jump to the latest SMS
                    addSMS(ThreadViewWindow::Action::NewestPage);
                    break;
                case db::Query::Type::Update:
                case db::Query::Type::Delete:
                    addSMS(ThreadViewWindow::Action::Refresh);
                    break;
                case db::Query::Type::Read:
                    // do not update view, as we don't have visual representation for read status
                    break;
                }
                if (this == application->getCurrentWindow()) {
                    application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                }
                return true;
            }
            if (msg->interface == db::Interface::Name::SMSThread) {
                if (msg->type == db::Query::Type::Delete) {
                    if (this == application->getCurrentWindow()) {
                        application->switchWindow(gui::name::window::main_window);
                    }
                }
            }
        }
        return false;
    }

} /* namespace gui */
