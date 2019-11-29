#include "PhonebookErrors.hpp"
#include "../ApplicationPhonebook.hpp"
#include "InputEvent.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "PhonebookContact.hpp"
#include "Text.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-db/api/DBServiceAPI.hpp"

using namespace ErrorWindows;

static Label *addLabel(gui::Item *parentPage, int x, int y, int w, int h, const std::string text = "", const std::string fontName = style::window::font::small,
                       const RectangleEdgeFlags edges = RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                       const Alignment alignment = Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM), const bool lineMode = false)
{
    Label *l = new Label(parentPage, x, y, w, h);
    l->setFilled(false);
    l->setBorderColor(ColorFullBlack);
    l->setEdges(edges);
    l->setFont(fontName);
    l->setText(text);
    l->setAlignement(alignment);
    l->setLineMode(lineMode);
    return (l);
}

NoResults::NoResults(app::Application *app) : AppWindow(app, "NoResults")
{
    setSize(style::window_width, style::window_height);
    buildInterface();
}

NoResults::~NoResults()
{
}

void NoResults::buildInterface()
{
    AppWindow::buildInterface();
    topBar->setActive(TopBar::Elements::TIME, true);

    bottomBar->setActive(BottomBar::Side::LEFT, false);
    bottomBar->setActive(BottomBar::Side::CENTER, false);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);

    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    titleLabel = addLabel(this, 0, 52, style::window_width, 35, utils::localize.get("app_phonebook_search_win_contacts"), style::window::font::small,
                          RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES, Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_BOTTOM));

    topSeparatorLabel = addLabel(this, 0, 104, style::window_width, 1, "", style::window::font::small, RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

    informationLabel = new Text(this, 45, 315, 390, 90);
    informationLabel->setText(utils::localize.get("app_phonebook_search_no_results"));
    informationLabel->setFillColor(ColorTray);
    informationLabel->setTextType(Text::TextType::MULTI_LINE);
    informationLabel->setEditMode(Text::EditMode::BROWSE);
    informationLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    informationLabel->setFont(style::window::font::small);

    searchCenter = new Image(this, 224, 265, 32, 32, "search");
}

void NoResults::rebuild()
{
}

void NoResults::destroyInterface()
{
    AppWindow::destroyInterface();
}

void NoResults::onBeforeShow(ShowMode mode, SwitchData *data)
{
    application->setKeyboardProfile(utils::localize.get("common_kbd_upper"));
}

bool NoResults::handleSwitchData(SwitchData *data)
{
    if (data == nullptr)
    {
        LOG_ERROR("Received null pointer");
        return false;
    }

    PhonebookSearchQuery *item = reinterpret_cast<PhonebookSearchQuery *>(data);
    searchQuery = item->getQuery();

    setContactData();

    return (true);
}

void NoResults::setContactData()
{
    titleLabel->setText(utils::localize.get("app_phonebook_search_results").c_str() + std::string(":\"") + searchQuery.c_str() + std::string("\""));
}

bool NoResults::onInput(const InputEvent &inputEvent)
{
    // check if any of the lower inheritance onInput methods catch the event
    LOG_INFO("onInput %d", inputEvent.keyCode);
    bool ret = AppWindow::onInput(inputEvent);

    if (ret)
    {
        // refresh window only when key is other than enter
        if (inputEvent.keyCode != KeyCode::KEY_ENTER)
        {
            application->render(RefreshModes::GUI_REFRESH_FAST);
        }
        return (true);
    }

    if ((inputEvent.state != InputEvent::State::keyReleasedShort) && ((inputEvent.state != InputEvent::State::keyReleasedLong)))
        return false;

    if (inputEvent.keyCode == KeyCode::KEY_RF)
    {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookSearchQuery>(searchQuery);
        application->switchWindow("Search", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        return (true);
    }
    else
    {
        LOG_INFO("unhandled event code=%d", inputEvent.keyCode);
    }
    return (false);
}

/*
 * contact blocked window
 */
ContactBlocked::ContactBlocked(app::Application *app) : AppWindow(app, "NoResults")
{
    setSize(style::window_width, style::window_height);
    buildInterface();
}

ContactBlocked::~ContactBlocked()
{
}

void ContactBlocked::buildInterface()
{
    AppWindow::buildInterface();
    topBar->setActive(TopBar::Elements::TIME, true);

    bottomBar->setActive(BottomBar::Side::LEFT, false);
    bottomBar->setActive(BottomBar::Side::CENTER, false);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);

    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    titleLabel = addLabel(this, 0, 52, style::window_width, 35, utils::localize.get("app_phonebook_search_win_contacts"), style::window::font::small,
                          RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES, Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_BOTTOM));

    topSeparatorLabel = addLabel(this, 0, 104, style::window_width, 1, "", style::window::font::small, RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

    informationLabel = new Text(this, 45, 315, 390, 90);
    informationLabel->setText(utils::localize.get("app_phonebook_search_no_results"));
    informationLabel->setFillColor(ColorTray);
    informationLabel->setTextType(Text::TextType::MULTI_LINE);
    informationLabel->setEditMode(Text::EditMode::BROWSE);
    informationLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    informationLabel->setFont(style::window::font::small);

    searchCenter = new Image(this, 224, 265, 32, 32, "search");
}

void ContactBlocked::rebuild()
{
}

void ContactBlocked::destroyInterface()
{
    AppWindow::destroyInterface();
}

void ContactBlocked::onBeforeShow(ShowMode mode, SwitchData *data)
{
    application->setKeyboardProfile(utils::localize.get("common_kbd_upper"));
}

bool ContactBlocked::handleSwitchData(SwitchData *data)
{
    if (data == nullptr)
    {
        LOG_ERROR("Received null pointer");
        return false;
    }

    PhonebookSearchQuery *item = reinterpret_cast<PhonebookSearchQuery *>(data);
    searchQuery = item->getQuery();

    setContactData();

    return (true);
}

void ContactBlocked::setContactData()
{
    titleLabel->setText(utils::localize.get("app_phonebook_search_results").c_str() + std::string(":\"") + searchQuery.c_str() + std::string("\""));
}

bool ContactBlocked::onInput(const InputEvent &inputEvent)
{
    // check if any of the lower inheritance onInput methods catch the event
    LOG_INFO("onInput %d", inputEvent.keyCode);
    bool ret = AppWindow::onInput(inputEvent);

    if (ret)
    {
        // refresh window only when key is other than enter
        if (inputEvent.keyCode != KeyCode::KEY_ENTER)
        {
            application->render(RefreshModes::GUI_REFRESH_FAST);
        }
        return (true);
    }

    if ((inputEvent.state != InputEvent::State::keyReleasedShort) && ((inputEvent.state != InputEvent::State::keyReleasedLong)))
        return false;

    if (inputEvent.keyCode == KeyCode::KEY_RF)
    {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookSearchQuery>(searchQuery);
        application->switchWindow("Search", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        return (true);
    }
    else
    {
        LOG_INFO("unhandled event code=%d", inputEvent.keyCode);
    }
    return (false);
}
