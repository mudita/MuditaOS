#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"

#include "i18/i18.hpp"

#include "Label.hpp"
#include "ListView.hpp"
#include "Margins.hpp"

#include "service-db/messages/DBMessage.hpp"
#include "service-db/api/DBServiceAPI.hpp"

#include <log/log.hpp>

#include "ThreadViewWindow.hpp"
#include <Style.hpp>

#include "../widgets/ThreadModel.hpp"

#include "../data/SMSdata.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"
#include <application-phonebook/data/PhonebookItemData.hpp>
#include <gui/widgets/Text.hpp>
#include <widgets/ListItem.hpp>
#include <widgets/ListItemProvider.hpp>
#include <widgets/ListView.hpp>

namespace gui
{
    class LolListItem : public gui::ListItem
    {
        gui::Label *body = nullptr;

      public:
        LolListItem(int i = 0)
        {
            { // copied from ThreadItem
                minWidth = 431;
                minHeight = 100;
                maxWidth = 431;
                maxHeight = 100;
                setRadius(0);
                setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
                setPenWidth(0);
                setPenFocusWidth(2);
            }

            setID(i);
            body = new gui::Label(this, gui::meta::Label({0, 0, 0, 0}));
            body->setText("Text: " + std::to_string(i));
        }

        // this won't work - copying label and other crap
        LolListItem(const LolListItem &) = default;

        bool onDimensionChanged(const gui::BoundingBox &oldDim, const gui::BoundingBox &newDim) override
        {
            LOG_INFO("Dim changed! %s : %d", body->getText().c_str(), getID());
            LOG_INFO("%x", body);
            body->setPosition(newDim.x, newDim.y);
            body->setSize(newDim.w, newDim.h);
            return true;
        }

        virtual bool onFocus(bool state) override
        {
            ListItem::onFocus(state);
            LOG_INFO("Focus changed for! %s", body->getText().c_str());
            return true;
        }
    };
}; // namespace gui

class Provider : public gui::ListItemProvider
{
    std::map<int, gui::LolListItem *> items;

  public:
    Provider(app::Application *app)
    {
    }

    virtual ~Provider() = default;

    gui::ListItem *getItem(int index, int firstElement, int prevIndex, uint32_t count, int remaining, bool topDown) override
    {
        LOG_INFO("getItem");
        if (index > getItemCount())
        {
            return nullptr;
        }
        if (items.find(index) == items.end())
        {
            items[index] = new gui::LolListItem(index);
        }
        // copy as getItem will push element onto list with parent..
        return new gui::LolListItem(*items[index]);
    }

    int getItemCount() const override
    {
        return 11;
    };
};

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
        body->setPenWidth(0);
        body->setPenFocusWidth(0);

        // new text, TODO TODO - send it and reload sms
        auto text = new gui::Text(nullptr, 0, 0, elements_width, 100, "", gui::Text::ExpandMode::EXPAND_UP);
        text->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc}, [=](const UTF8 &text) { textModeShowCB(text); }, [=]() { textSelectSpecialCB(); }));
        text->setPenFocusWidth(2);
        text->setPenWidth(2);
        text->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        // TODO IFS
        text->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("Send SMS callback");
            CellularServiceAPI::SendSMS(application, title->getText(), text->getText());
            return true;
        };

        body->tryAddWidget(text);
        /// setup
        body->setReverseOrder(true);
        body->setVisible(true);
        setFocusItem(body);
    }

    void ThreadViewWindow::cleanMessages()
    {
        // hack just to fix setting pos for now
        if (body->children.size() > 1)
        {
            std::for_each(std::next(body->children.begin(), 1), body->children.end(), [=](auto el) { body->removeWidget(el); });
        }
    }

    void ThreadViewWindow::addMessages(uint32_t thread_id)
    {
        auto elements_width = this->getWidth() - style::window::default_left_margin * 2;
        LOG_INFO("=============================");
        std::unique_ptr<std::vector<SMSRecord>> sms = DBServiceAPI::SMSGetLimitOffsetByThreadID(application, 0, 4, thread_id);
        LOG_INFO("=============================");
        /// dummy sms thread - TODO TODO load from db - on switchData
        /// this is not 'static' do it on window focus (on swtich window)
        auto labelmeta = gui::meta::Label({0, 0, elements_width, 100});
        labelmeta.edges = RectangleEdgeFlags::GUI_RECT_ALL_EDGES;
        labelmeta.radius = 3;
        labelmeta.align = gui::Alignment::ALIGN_VERTICAL_TOP;
        labelmeta.focus = 3;
        labelmeta.no_focus = 1;
        labelmeta.margins = Margins(0, 2, 0, 2);
        // hack - tmp to fix fitting
        body->setReverseOrder(false);
        for (auto &el : *sms)
        {
            auto label = new gui::Label(nullptr, labelmeta);
            label->setText(el.body);
            LOG_INFO("Add sms: %s %s", el.body.c_str(), el.number.c_str());
            if (body->tryAddWidget(label)) {}
            else
            {
                delete label;
                break;
            }
        }
        body->setReverseOrder(true);
        body->setNavigation();
    }

    void ThreadViewWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
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
                // cleanMessages
                addMessages(pdata->thread->dbID);
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
                LOG_DEBUG("SEND SMS TO: %s %s %s %s %s", pdata->contact->number.c_str(), pdata->contact->numbers[0].numberE164.c_str(),
                          pdata->contact->numbers[0].numberUser.c_str(), pdata->contact->primaryName.c_str(), pdata->contact->alternativeName.c_str());
                setTitle(pdata->contact->numbers[0].numberUser);
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
