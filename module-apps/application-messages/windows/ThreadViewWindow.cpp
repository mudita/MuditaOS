#include "ThreadViewWindow.hpp"

#include "../ApplicationMessages.hpp"
#include "../data/SMSdata.hpp"
#include "OptionsMessages.hpp"
#include "Service/Message.hpp"

#include <Text.hpp>
#include <TextBubble.hpp>
#include <Label.hpp>
#include <Margins.hpp>
#include <service-db/api/DBServiceAPI.hpp>
#include <service-appmgr/ApplicationManager.hpp>
#include <service-db/messages/DBNotificationMessage.hpp>
#include <i18/i18.hpp>
#include <time/time_conversion.hpp>
#include <log/log.hpp>
#include <Style.hpp>
#include <OptionWindow.hpp>

#include <memory>
#include <cassert>

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
        if (inputMessage != nullptr) {
            return;
        }

        inputMessage = new SMSInputWidget(body, application);

        inputMessage->activatedCallback = [&](gui::Item &item) {
            auto app = dynamic_cast<app::ApplicationMessages *>(application);
            assert(app != nullptr);
            if (app->handleSendSmsFromThread(*number, inputMessage->inputText->getText())) {
                LOG_ERROR("handleSendSmsFromThread failed");
            }
            inputMessage->inputText->clear();
            return true;
        };
    }

    void ThreadViewWindow::destroyTextItem()
    {
        body->erase(inputMessage);
        if (inputMessage->parent == nullptr) {
            delete (inputMessage);
        }
        inputMessage = nullptr;
    }

    void ThreadViewWindow::cleanView()
    {
        body->removeWidget(inputMessage);
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
        // 1. load elements to tmp vector
        std::unique_ptr<ThreadRecord> threadDetails = DBServiceAPI::ThreadGet(this->application, SMS.thread);
        if (threadDetails == nullptr) {
            LOG_ERROR("cannot fetch details of selected thread (id: %d)", SMS.thread);
            return;
        }
        SMS.dbsize = threadDetails->msgCount;

        if (threadDetails->isUnread()) {
            auto app = dynamic_cast<app::ApplicationMessages *>(application);
            assert(app != nullptr);
            if (application->getCurrentWindow() == this) {
                app->markSmsThreadAsRead(threadDetails->ID);
            }
        }

        LOG_DEBUG("start: %d end: %d db: %d", SMS.start, SMS.end, SMS.dbsize);
        if (what == Action::Init || what == Action::NewestPage) {
            SMS.start = 0;

            // Refactor
            SMS.end = maxsmsinwindow;
            if (what == Action::Init) {
                destroyTextItem();
            }
            refreshTextItem();
        }

        // 2. check how many of these will fit in box
        //         update begin / end in `SMS`
        if (what == Action::NextPage) {
            if (SMS.end != SMS.dbsize) {

                // Refactor
                for (auto sms : body->children) {
                    if (sms->visible)
                        SMS.start++;
                }

                if (inputMessage->visible)
                    SMS.start -= 1;

                LOG_INFO("SMS start %d", SMS.start);
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
            LOG_WARN("Malformed thread. Leave it (id: %d)", SMS.thread);
            application->switchWindow(gui::name::window::main_window);
            return;
        }
        // 3. add them to box
        this->cleanView();
        // if we are going from 0 then we want to show text prompt
        if (SMS.start == 0) {
            body->addWidget(inputMessage);
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
        labelSpan->setMaximumHeight(smsBubble->widgetMaximumArea.h);
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
            smsBubble->setPadding(style::window::messages::sms_left_bubble_padding);
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
            addErrorIcon(labelSpan);
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

    void ThreadViewWindow::addErrorIcon(HBox *layout) const
    {
        auto errorIcon = new gui::Image("messages_error_W_M");
        errorIcon->setAlignment(Alignment(Alignment::Vertical::Center));
        errorIcon->activeItem = false; // make it non-focusable
        errorIcon->setMargins(Margins(style::window::messages::sms_error_icon_offset,
                                      0,
                                      (style::window::messages::sms_failed_offset -
                                       (errorIcon->getWidth() + style::window::messages::sms_error_icon_offset)),
                                      0));
        layout->addWidget(errorIcon);
    }

    void ThreadViewWindow::addTimeLabel(HBox *layout, Label *timeLabel, uint16_t widthAvailable) const
    {
        // add time label activated on focus
        timeLabel->setMinimumWidth(timeLabel->getTextNeedSpace());
        timeLabel->setMinimumHeight(layout->getHeight());
        timeLabel->setSize(timeLabel->getTextNeedSpace(), layout->getHeight());

        uint16_t timeLabelSpacerWidth = widthAvailable - timeLabel->getWidth();

        timeLabel->setMargins(Margins(timeLabelSpacerWidth, 0, timeLabelSpacerWidth, 0));
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
        timeLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
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
        smsBubble->setPadding(style::window::messages::sms_right_bubble_padding);
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

        // wrap label in H box, to make fit datetime in it
        HBox *labelSpan = smsSpanBuild(smsBubble, smsRecord);
        labelSpan->setMargins(Margins(0, style::window::messages::sms_vertical_spacer, 0, 0));

        if (labelSpan == nullptr) {
            return false;
        }

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
                auto ret = DBServiceAPI::ContactGetByIDWithTemporary(application, pdata->thread->contactID);
                contact  = std::make_shared<ContactRecord>(ret->front());
                // should be name number for now - easier to handle
                setTitle(ret->front().getFormattedName());
                auto retNumber = DBServiceAPI::GetNumberById(application, pdata->thread->numberID, numberIdTimeout);
                assert(retNumber != nullptr);
                number = std::move(retNumber);
                LOG_INFO("Phone number for thread: %s", number->getFormatted().c_str());
            }
        }
        if (auto pdata = dynamic_cast<SMSTextData *>(data)) {
            auto txt = pdata->text;
            LOG_INFO("received sms templates data \"%s\"", txt.c_str());
            pdata->concatenate == SMSTextData::Concatenate::True ? inputMessage->inputText->addText(txt)
                                                                 : inputMessage->inputText->setText(txt);
            body->resizeItems();
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
