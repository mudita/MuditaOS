#include "PhonebookMainWindow.hpp"

#include <functional>
#include <log/log.hpp>
#include <memory>

#include "../ApplicationPhonebook.hpp"
#include "Label.hpp"
#include "ListView.hpp"
#include "Margins.hpp"
#include "PhonebookMainWindow.hpp"
#include "i18/i18.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "service-db/messages/DBMessage.hpp"

namespace gui
{

PhonebookMainWindow::PhonebookMainWindow(app::Application *app)
    : AppWindow(app, "MainWindow"), phonebookModel{new PhonebookModel(app)}
{
    setSize(480, 600);
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

    title = new gui::Label(this, 0, 50, 480, 54);
    title->setFilled(false);
    title->setBorderColor(gui::ColorFullBlack);
    title->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
    title->setMargins(Margins(0, 0, 0, 18));
    title->setFont("gt_pressura_bold_24");
    title->setText(utils::localize.get("app_phonebook_title_main"));
    title->setAlignement(
        gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

    leftArrowImage = new gui::Image(this, 30, 62, 0, 0, "arrow_left");
    rightArrowImage = new gui::Image(this, 480 - 30 - 13, 62, 0, 0, "arrow_right");
    newContactImage = new gui::Image(this, 48, 55, 0, 0, "cross");
    searchImage = new gui::Image(this, 480 - 48 - 26, 55, 0, 0, "search");
}

void PhonebookMainWindow::destroyInterface()
{
    AppWindow::destroyInterface();
    if (title)
    {
        removeWidget(title);
        delete title;
        title = nullptr;
    }
    if (list)
    {
        removeWidget(list);
        delete list;
        list = nullptr;
    }
    if (leftArrowImage)
    {
        removeWidget(leftArrowImage);
        delete leftArrowImage;
        leftArrowImage = nullptr;
    }
    if (rightArrowImage)
    {
        removeWidget(rightArrowImage);
        delete rightArrowImage;
        rightArrowImage = nullptr;
    }
    if (newContactImage)
    {
        removeWidget(newContactImage);
        delete newContactImage;
        newContactImage = nullptr;
    }
    if (searchImage)
    {
        removeWidget(searchImage);
        delete searchImage;
        searchImage = nullptr;
    }

    children.clear();
    delete phonebookModel;
}

PhonebookMainWindow::~PhonebookMainWindow()
{
    destroyInterface();
}

void PhonebookMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    setFocusItem(list);

    phonebookModel->clear();
    phonebookModel->requestRecordsCount();

    list->clear();
    list->setElementsCount(phonebookModel->getItemCount());
}

bool PhonebookMainWindow::onInput(const InputEvent &inputEvent)
{
    // check if any of the lower inheritance onInput methods catch the event
    bool ret = AppWindow::onInput(inputEvent);
    if (ret)
    {
        // refresh window only when key is other than enter
        if (inputEvent.keyCode != KeyCode::KEY_ENTER)
            application->render(RefreshModes::GUI_REFRESH_FAST);
        return true;
    }

    // process only if key is released
    if ((inputEvent.state != InputEvent::State::keyReleasedShort) &&
        ((inputEvent.state != InputEvent::State::keyReleasedLong)))
        return false;

    if (inputEvent.keyCode == KeyCode::KEY_LEFT)
    {
        LOG_INFO("Adding new contact");
        application->switchWindow("New", gui::ShowMode::GUI_SHOW_INIT, nullptr);
    }
    else if (inputEvent.keyCode == KeyCode::KEY_RIGHT)
    {
        LOG_INFO("Switch to search view");
        application->switchWindow("Search", gui::ShowMode::GUI_SHOW_INIT, nullptr);
    }
    else if (inputEvent.keyCode == KeyCode::KEY_RF)
    {
        sapm::ApplicationManager::messageSwitchApplication(application, "ApplicationDesktop", "MenuWindow", nullptr);
        return (true);
    }

    return false;
}

bool PhonebookMainWindow::onDatabaseMessage(sys::Message *msgl)
{
    DBContactResponseMessage *msg = reinterpret_cast<DBContactResponseMessage *>(msgl);
    if (phonebookModel->updateRecords(std::move(msg->records), msg->offset, msg->limit, msg->count, msg->favourite))
        return true;

    return false;
}

} /* namespace gui */
