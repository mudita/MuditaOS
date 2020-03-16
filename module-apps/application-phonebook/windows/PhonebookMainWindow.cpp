#include "PhonebookMainWindow.hpp"
#include "../ApplicationPhonebook.hpp"
#include "Label.hpp"
#include "ListView.hpp"
#include "Margins.hpp"
#include "PhonebookNewContact.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "i18/i18.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "service-db/messages/DBMessage.hpp"
#include <Style.hpp>
#include <functional>
#include <log/log.hpp>
#include <memory>

namespace gui
{
PhonebookMainWindow::PhonebookMainWindow(app::Application *app) : AppWindow(app, gui::name::window::main_window), phonebookModel{new PhonebookModel(app)}
{
    buildInterface();
}

    void PhonebookMainWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

void PhonebookMainWindow::buildInterface()
{
    AppWindow::buildInterface();

    topBar->setActive(TopBar::Elements::TIME, true);
    setTitle(utils::localize.get("app_phonebook_title_main"));
    leftArrowImage = new gui::Image(this, 30, 62, 11, 13, "arrow_left");
    rightArrowImage = new gui::Image(this, 480 - 30 - 11, 62, 11, 13, "arrow_right");
    newContactImage = new gui::Image(this, 50, 55, 24, 24, "cross");
    searchImage = new gui::Image(this, 480 - 30 - 11 - 8 - 26, 55, 26, 26, "search");

    contactsList = new gui::PhonebookListView(this, 30, 105 + 6, 480 - 30 - 30, 600 - 105 + 6 - 50);
    contactsList->setMaxElements(7);
    contactsList->setPageSize(7);
    contactsList->setPenFocusWidth(0);
    contactsList->setPenWidth(0);
    contactsList->setProvider(phonebookModel);
    contactsList->setApplication(application);

    bottomBar->setActive(BottomBar::Side::LEFT, true);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_phonebook_call"));
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

}

void PhonebookMainWindow::destroyInterface()
{
    AppWindow::destroyInterface();

    removeWidget(contactsList);
    delete contactsList;
    contactsList = nullptr;

        removeWidget(list);
        delete list;
        list = nullptr;

        removeWidget(leftArrowImage);
        delete leftArrowImage;
        leftArrowImage = nullptr;

        removeWidget(rightArrowImage);
        delete rightArrowImage;
        rightArrowImage = nullptr;

        removeWidget(newContactImage);
        delete newContactImage;
        newContactImage = nullptr;

        removeWidget(searchImage);
        delete searchImage;
        searchImage = nullptr;

        children.clear();
        delete phonebookModel;
    }

<<<<<<< HEAD
    PhonebookMainWindow::~PhonebookMainWindow()
    {
        destroyInterface();
    }
=======
    void PhonebookMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        LOG_INFO("onBeforeShow");
        setFocusItem(list);
<<<<<<< HEAD
        phonebookModel->clear();
        phonebookModel->requestRecordsCount();
=======
    contactsList->clear();
    contactsList->setElementsCount(phonebookModel->getItemCount());
}
>>>>>>> [EGD-2932] Add PhonebookMAinWindow styles to PhonebookStyle.hpp

        list->clear();
        list->setElementsCount(phonebookModel->getItemCount());
    }

    bool PhonebookMainWindow::onInput(const InputEvent &inputEvent)
    {
        // process only if key is released
        if (inputEvent.state == InputEvent::State::keyReleasedShort) {
            switch (inputEvent.keyCode) {
            case KeyCode::KEY_LEFT: {
                std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>();
                application->switchWindow(gui::window::name::newContact, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            }
                return true;
            case KeyCode::KEY_RIGHT:
                application->switchWindow("Search");
                return true;
            default:
                break;
            }
        }

        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    bool PhonebookMainWindow::onDatabaseMessage(sys::Message *msgl)
    {
        DBContactResponseMessage *msg = reinterpret_cast<DBContactResponseMessage *>(msgl);
        if (phonebookModel->updateRecords(std::move(msg->records), msg->offset, msg->limit, msg->count, msg->favourite))
            return true;

        return false;
    }

} /* namespace gui */
