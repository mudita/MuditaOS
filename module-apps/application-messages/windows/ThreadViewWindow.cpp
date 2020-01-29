#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"

#include "i18/i18.hpp"
#include "time/time_conversion.hpp"

#include "Label.hpp"
#include "ListView.hpp"
#include "Margins.hpp"

#include "service-db/messages/DBMessage.hpp"

#include <log/log.hpp>

#include "ThreadViewWindow.hpp"
#include <Style.hpp>

#include "../widgets/ThreadModel.hpp"

#include "../ApplicationMessages.hpp"
#include "../data/SMSdata.hpp"
#include "OptionsMessages.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"
#include <application-phonebook/data/PhonebookItemData.hpp>
#include <gui/widgets/Text.hpp>
#include <widgets/ListItem.hpp>
#include <widgets/ListItemProvider.hpp>
#include <widgets/ListView.hpp>

#include "service-db/api/DBServiceAPI.hpp"

namespace style
{

}; // namespace style

namespace gui
{

    ThreadViewWindow::ThreadViewWindow(app::Application *app) : AppWindow(app, name::window::thread_view)
    {
        AppWindow::buildInterface();
        setTitle(utils::localize.get("app_messages_title_main"));
        topBar->setActive(TopBar::Elements::TIME, true);
        bottomBar->setActive(BottomBar::Side::LEFT, true);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("common_options"));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_send"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));
        auto elements_width = this->getWidth() - style::window::default_left_margin * 2;
        body = new gui::VBox(this, style::window::default_left_margin, title->offset_h(), elements_width, bottomBar->getY() - title->offset_h());
        body->setPenWidth(style::window::default_border_no_focus_w);
        body->setPenFocusWidth(style::window::default_border_no_focus_w);
        body->borderCallback = [this](const InputEvent &inputEvent) -> bool {
            if (inputEvent.state != InputEvent::State::keyReleasedShort)
            {
                return false;
            }
            if (inputEvent.keyCode == KeyCode::KEY_UP)
            {
                return this->showMessages(Action::Next);
            }
            else if (inputEvent.keyCode == KeyCode::KEY_DOWN)
            {
                return this->showMessages(Action::Previous);
            }
            else
            {
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
        if (text)
        {
            body->removeWidget(text);
        }
        text = new gui::Text(nullptr, 0, 0, body->getWidth(), style::window::messages::sms_height, "", gui::Text::ExpandMode::EXPAND_UP);
        text->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc}, [=](const UTF8 &text) { textModeShowCB(text); }, [=]() { textSelectSpecialCB(); }));
        text->setPenFocusWidth(style::window::default_border_focucs_w);
        text->setPenWidth(style::window::default_border_focucs_w);
        text->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        text->activatedCallback = [&](gui::Item &item) {
            if (text->getText().length() == 0)
            {
                LOG_DEBUG("No text to send in SMS");
                return true;
            }
            SMSRecord record;
            record.number = title->getText();
            record.number = UTF8("+48666600925");
            record.body = text->getText();
            record.type = SMSType::QUEUED;
            auto time = utils::time::Timestamp();
            record.date = time.getTime();
            DBServiceAPI::SMSAdd(this->application, record);

            return true;
        };
    }

    void ThreadViewWindow::cleanView()
    {
        body->removeWidget(text);
        body->setFocusItem(nullptr);
        std::for_each(body->children.begin(), body->children.end(), [&](auto &el) { delete el; });
        body->children.erase(body->children.begin(), body->children.end());
    }

    bool ThreadViewWindow::showMessages(ThreadViewWindow::Action what)
    {
        if (SMS.thread <= 0)
        {
            LOG_ERROR("threadID not set!");
            return false;
        }
        addSMS(what);
        return true;
    }

    void ThreadViewWindow::addSMS(ThreadViewWindow::Action what)
    {
        LOG_DEBUG("--- %d ---", what);
        // if there was text - then remove it temp
        gui::Alignment align;
        // 1. load elements to tmp vector
        SMS.dbsize = DBServiceAPI::SMSGetCount(this->application);
        LOG_DEBUG("start: %d end: %d db: %d", SMS.start, SMS.end, SMS.dbsize);
        if (what == Action::Start)
        {
            SMS.start = 0;
            SMS.end = maxsmsinwindow;
            rebuildText();
        }

        // TODO 2. check how many of these will fit in box
        //         update begin / end in `SMS`
        if (what == Action::Next)
        {
            if (SMS.end != SMS.dbsize)
            {
                SMS.start = SMS.end;
            }
            else
            {
                LOG_INFO("All sms shown");
                return;
            }
        }
        else if (what == Action::Previous)
        {
            if (SMS.start == 0)
            {
                return;
            }
            else if (SMS.start - maxsmsinwindow < 0)
            {
                SMS.start = 0;
            }
            else
            {
                SMS.start -= maxsmsinwindow;
            }
            LOG_DEBUG("in progress %d", SMS.start);
        }
        SMS.sms = DBServiceAPI::SMSGetLimitOffsetByThreadID(this->application, SMS.start, maxsmsinwindow, SMS.thread);
        LOG_DEBUG("=> SMS %d < %d < %d", SMS.start, SMS.sms->size(), maxsmsinwindow);
        // 3. add them to box
        this->cleanView();
        // if we are going from 0 then we want to show text prompt
        if (SMS.start == 0)
        {
            body->addWidget(text);
        }

        SMS.end = SMS.start;
        for (auto &el : *SMS.sms)
        {
            LOG_DEBUG("...");
            if (!smsBuild(el, false))
            {
                break;
            }
            ++SMS.end;
        }
        body->setNavigation();
        setFocusItem(body);
        if (Action::Previous == what)
        {
            body->setVisible(true, true);
        }
        LOG_DEBUG("sms built");
    }

    bool ThreadViewWindow::smsBuild(const SMSRecord &el, bool bottom)
    {
        /// dummy sms thread - TODO load from db - on switchData
        auto label = new Text(nullptr, 0, 1, 300, 5);
        label->setTextType(Text::TextType::MULTI_LINE);
        label->setEditMode(Text::EditMode::SCROLL);
        label->setEdges(RectangleEdgeFlags::GUI_RECT_ALL_EDGES);
        label->setRadius(5);
        label->setFont(style::window::font::medium);
        label->setPenFocusWidth(style::window::default_border_focucs_w);
        label->setPenWidth(style::window::messages::sms_border_no_focus);
        label->expandMode = Text::ExpandMode::EXPAND_DOWN;
        label->buildDrawList();
        label->setText(el.body.length() ? el.body : " "); // text doesn't really like being empty
        // TODO move to other decorating function depending on what happens (bind left, right, add dashdot etc)
        LOG_DEBUG("ADD SMS TYPE: %d", el.type);
        switch (el.type)
        {
        case SMSType::OUTBOX:
            label->setYaps(RectangleYapFlags::GUI_RECT_YAP_TOP_RIGHT);
            label->setX(body->getWidth() - label->getWidth());
            break;
        case SMSType::INBOX:
            label->setYaps(RectangleYapFlags::GUI_RECT_YAP_TOP_LEFT);
            break;
        default:
            break;
        }
        // @TODO: need for a callback yap change. yaps don't call recalculateDrawParams() because they are not Label, but Rect
        label->setMargins(gui::Margins(10, 10, 10, 10)); // needs to be after setYaps, because see above ↑
        label->activatedCallback = [=](Item &) {
            LOG_INFO("Message activated!");
            auto app = dynamic_cast<app::ApplicationMessages *>(application);
            if (app == nullptr)
            {
                LOG_ERROR("Something went horribly wrong");
                return false;
            }
            if (app->windowOptions != nullptr)
            {
                app->windowOptions->clearOptions();
                app->windowOptions->addOptions(smsWindowOptions(app, el));
                app->switchWindow(app->windowOptions->getName(), nullptr);
            }
            return true;
        };
        auto rect = new gui::Rect(nullptr, 0, 0, 10, 10);
        rect->activeItem = false;
        rect->setPenWidth(0);
        body->addWidget(rect);
        LOG_INFO("Add sms: %s %s", el.body.c_str(), el.number.c_str());
        body->addWidget(label);
        return label->visible;
    }

    void ThreadViewWindow::rebuild()
    {
        addSMS(ThreadViewWindow::Action::Start);
    }

    void ThreadViewWindow::buildInterface()
    {
    }

    void ThreadViewWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
        children.clear();
    }

    ThreadViewWindow::~ThreadViewWindow()
    {
        destroyInterface();
    }

    void ThreadViewWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        {
            auto pdata = dynamic_cast<SMSThreadData *>(data);
            if (pdata)
            {
                LOG_INFO("We have it! %d", pdata->thread->dbID);
                cleanView();
                SMS.thread = pdata->thread->dbID;
                showMessages(Action::Start);
                auto ret = DBServiceAPI::ContactGetByID(application, pdata->thread->contactID);
                // should be name number for now - easier to handle
                setTitle(ret->front().number);
                return;
            }
        }
        {
            auto pdata = dynamic_cast<SMSSendRequest *>(data);
            if (pdata)
            {
                LOG_INFO("Phonebook sms send request!");
                // TODO agree what should be used and how. Now Request have only contact,
                // maybe it should have additional info - which nr to use and how to show it
                if (pdata->contact->numbers.size() != 0)
                {
                    LOG_DEBUG("SEND SMS TO: %s %s %s %s %s", pdata->contact->number.c_str(), pdata->contact->numbers[0].numberE164.c_str(),
                              pdata->contact->numbers[0].numberUser.c_str(), pdata->contact->primaryName.c_str(), pdata->contact->alternativeName.c_str());
                    setTitle(pdata->contact->numbers[0].numberUser);
                }
                else
                {
                    // TODO handle error better
                    setTitle("NO CONTACT");
                }
            }
        }
    }

    bool ThreadViewWindow::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }

    bool ThreadViewWindow::onDatabaseMessage(sys::Message *msgl)
    {
        //	DBContactResponseMessage* msg = reinterpret_cast<DBContactResponseMessage*>( msgl );
        //	if( phonebookModel->updateRecords( std::move(msg->records), msg->offset, msg->limit, msg->count, msg->favourite ) )
        //		return true;

        return false;
    }

} /* namespace gui */
