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

namespace gui
{

    static Label *addLabel(gui::Item *parentPage,
                           int x,
                           int y,
                           int w,
                           int h,
                           const std::string text         = "",
                           const std::string fontName     = style::window::font::small,
                           const RectangleEdgeFlags edges = RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                           const Alignment alignment      = Alignment(Alignment::ALIGN_HORIZONTAL_LEFT,
                                                                 Alignment::ALIGN_VERTICAL_BOTTOM),
                           const bool lineMode            = false)
    {
        Label *l = new Label(parentPage, x, y, w, h);
        l->setFilled(false);
        l->setBorderColor(ColorFullBlack);
        l->setEdges(edges);
        l->setFont(fontName);
        l->setText(text);
        l->setAlignment(alignment);
        l->setLineMode(lineMode);
        return (l);
    }

    NoResults::NoResults(app::Application *app) : AppWindow(app, gui::window::name::no_results)
    {
        buildInterface();
    }

    NoResults::~NoResults()
    {}

    void NoResults::buildInterface()
    {
        AppWindow::buildInterface();
        topBar->setActive(TopBar::Elements::TIME, true);

        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, false);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);

        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

        topSeparatorLabel = addLabel(this,
                                     0,
                                     104,
                                     style::window_width,
                                     1,
                                     "",
                                     style::window::font::small,
                                     RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

        informationLabel = new Text(this, 45, 315, 390, 90);
        informationLabel->setText(utils::localize.get("app_phonebook_search_no_results"));
        informationLabel->setFillColor(ColorTray);
        informationLabel->setTextType(TextType::MULTI_LINE);
        informationLabel->setEditMode(EditMode::BROWSE);
        informationLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        informationLabel->setFont(style::window::font::small);

        searchCenter = new Image(this, 224, 265, 32, 32, "search");
    }

    void NoResults::rebuild()
    {}

    void NoResults::destroyInterface()
    {
        AppWindow::destroyInterface();
    }

    bool NoResults::handleSwitchData(SwitchData *data)
    {
        PhonebookSearchQuery *item = dynamic_cast<PhonebookSearchQuery *>(data);
        if (item == nullptr) {
            LOG_WARN("Received null pointer");
            return false;
        }

        searchQuery = item->getQuery();

        setContactData();

        return (true);
    }

    void NoResults::setContactData()
    {
        setTitle(utils::localize.get(style::strings::common::search_results) + std::string(":\"") + searchQuery +
                 std::string("\""));
    }

    bool NoResults::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.keyCode == KeyCode::KEY_RF && (inputEvent.state == InputEvent::State::keyReleasedShort)) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookSearchQuery>(searchQuery);
            application->switchWindow("Search", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return (true);
        }

        return (AppWindow::onInput(inputEvent));
    }

    /*
     * contact blocked window
     */
    ContactBlocked::ContactBlocked(app::Application *app) : AppWindow(app, gui::window::name::contact_blocked)
    {
        buildInterface();
    }

    ContactBlocked::~ContactBlocked()
    {}

    void ContactBlocked::buildInterface()
    {
        AppWindow::buildInterface();
        topBar->setActive(TopBar::Elements::TIME, true);

        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, false);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);

        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));
        topSeparatorLabel = addLabel(this,
                                     0,
                                     104,
                                     style::window_width,
                                     1,
                                     "",
                                     style::window::font::small,
                                     RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

        informationLabel = new Text(this, 45, 315, 390, 90);
        informationLabel->setText(utils::localize.get("app_phonebook_search_no_results"));
        informationLabel->setFillColor(ColorTray);
        informationLabel->setTextType(TextType::MULTI_LINE);
        informationLabel->setEditMode(EditMode::BROWSE);
        informationLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        informationLabel->setFont(style::window::font::small);

        searchCenter = new Image(this, 224, 265, 32, 32, "search");
    }

    void ContactBlocked::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void ContactBlocked::destroyInterface()
    {
        AppWindow::destroyInterface();
    }

    bool ContactBlocked::handleSwitchData(SwitchData *data)
    {
        PhonebookSearchQuery *item = dynamic_cast<PhonebookSearchQuery *>(data);
        if (item == nullptr) {
            LOG_WARN("Received null pointer");
            return false;
        }

        searchQuery = item->getQuery();

        setContactData();

        return (true);
    }

    void ContactBlocked::setContactData()
    {
        setTitle(utils::localize.get(style::strings::common::search_results) + std::string(":\"") + searchQuery +
                 std::string("\""));
    }

    bool ContactBlocked::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.keyCode == KeyCode::KEY_RF && (inputEvent.state == InputEvent::State::keyReleasedShort)) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookSearchQuery>(searchQuery);
            application->switchWindow("Search", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return (true);
        }

        return (AppWindow::onInput(inputEvent));
    }
} // namespace gui
