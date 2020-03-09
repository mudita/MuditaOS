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

    list = new gui::PhonebookListView(this, 11, 105, 480 - 22, 600 - 105 - 50);
    list->setMaxElements(7);
    list->setPageSize(7);
    list->setPenFocusWidth(0);
    list->setPenWidth(0);
    list->setProvider(phonebookModel);
    list->setApplication(application);

    bottomBar->setActive(BottomBar::Side::LEFT, true);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_phonebook_call"));
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    topBar->setActive(TopBar::Elements::TIME, true);

    setTitle(utils::localize.get("app_phonebook_title_main"));

    leftArrowImage = new gui::Image(this, 30, 62, 0, 0, "arrow_left");
    rightArrowImage = new gui::Image(this, 480 - 30 - 13, 62, 0, 0, "arrow_right");
    newContactImage = new gui::Image(this, 48, 55, 0, 0, "cross");
    searchImage = new gui::Image(this, 480 - 48 - 26, 55, 0, 0, "search");
}

void PhonebookMainWindow::destroyInterface()
{
    AppWindow::destroyInterface();

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

PhonebookMainWindow::~PhonebookMainWindow()
{
    destroyInterface();
}

void PhonebookMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    LOG_INFO("onBeforeShow");
    setFocusItem(list);

    phonebookModel->clear();
    phonebookModel->requestRecordsCount();

    list->clear();
    list->setElementsCount(phonebookModel->getItemCount());
}

bool PhonebookMainWindow::onInput(const InputEvent &inputEvent)
{
    // process only if key is released
    if (inputEvent.state == InputEvent::State::keyReleasedShort)
    {
        switch (inputEvent.keyCode)
        {
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
