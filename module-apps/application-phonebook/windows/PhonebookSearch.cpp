#include "PhonebookSearch.hpp"
#include "../ApplicationPhonebook.hpp"
#include "InputEvent.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "PhonebookContact.hpp"
#include "Text.hpp"
#include "Utils.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-db/api/DBServiceAPI.hpp"

PhonebookSearch::PhonebookSearch(app::Application *app)
    : AppWindow(app, "Search"), phonebookModel{new PhonebookModel(app)}
{
    setSize(480, 600);
    buildInterface();
}

PhonebookSearch::~PhonebookSearch()
{
}

Label *PhonebookSearch::addLabel(std::list<Item *> *parentPage, int x, int y, int w, int h, const std::string text = "",
                                 const std::string fontName = style::window::font::small,
                                 const RectangleEdgeFlags edges = RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                                 const Alignment alignment = Alignment(Alignment::ALIGN_HORIZONTAL_LEFT,
                                                                       Alignment::ALIGN_VERTICAL_BOTTOM),
                                 const bool lineMode = false)
{
    Label *l = new Label(this, x, y, w, h);
    l->setFilled(false);
    l->setBorderColor(ColorFullBlack);
    l->setEdges(edges);
    l->setFont(fontName);
    l->setText(text);
    l->setAlignement(alignment);
    l->setLineMode(lineMode);

    // addItem(l);
    if (parentPage)
        parentPage->push_back(l);

    return (l);
}

void PhonebookSearch::buildInterface()
{
    AppWindow::buildInterface();
    topBar->setActive(TopBar::Elements::TIME, true);

    bottomBar->setActive(BottomBar::Side::LEFT, false);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);

    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_search"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    titleLabel = addLabel(nullptr, 0, 52, 480, 35, utils::localize.get("app_phonebook_search_win_contacts"),
                          style::header::font::title, RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                          Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_BOTTOM));

    topSeparatorLabel = addLabel(nullptr, 0, 104, 480, 1, "", "", RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
    searchHeader = addLabel(nullptr, 30, 127, 90, 20, utils::localize.get("app_phonebook_search_win_search"),
                            "gt_pressura_regular_18");

    informationLabel = new Text(this, 45, 315, 390, 90);
    informationLabel->setText(utils::localize.get("app_phonebook_search_win_info"));
    informationLabel->setFillColor(ColorTray);
    informationLabel->setTextType(Text::TextType::MULTI_LINE);
    informationLabel->setEditMode(Text::EditMode::BROWSE);
    informationLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    // informationLabel->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_BOTTOM));
    informationLabel->setFont(style::window::font::small);

    inputField = new Text(this, 30, 153, 420, 33);
    inputField->setTextType(Text::TextType::SINGLE_LINE);
    inputField->setPenWidth(3);
    inputField->setEditMode(Text::EditMode::EDIT);
    inputField->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
    // inputField->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM));
    inputField->setFont(style::window::font::small);

    searchTop = new Image(this, 420, 153, 32, 32, "search");
    searchCenter = new Image(this, 224, 265, 32, 32, "search");
    fancyArrowDown = new Image(this, 174, 380, 132, 132, "fancy_arrow_down_big");

    searchResultList = new gui::PhonebookListView(this, 11, 105, 480 - 22, 600 - 105 - 50);
    searchResultList->setMaxElements(7);
    searchResultList->setPageSize(7);
    searchResultList->setPenFocusWidth(0);
    searchResultList->setPenWidth(0);
    searchResultList->setProvider(phonebookModel);
    searchResultList->setApplication(application);

    setFocusItem(inputField);
}

void PhonebookSearch::rebuild()
{
}

void PhonebookSearch::destroyInterface()
{
    AppWindow::destroyInterface();
}

void PhonebookSearch::onBeforeShow(ShowMode mode, SwitchData *data)
{
    setFocusItem(inputField);
    application->setKeyboardProfile(utils::localize.get("common_kbd_upper"));
}

bool PhonebookSearch::handleSwitchData(SwitchData *data)
{
    if (data == nullptr)
    {
        LOG_ERROR("Received null pointer");
        return false;
    }

    PhonebookSearchQuery *item = reinterpret_cast<PhonebookSearchQuery *>(data);
    inputField->setText(item->getQuery());

    return (true);
}

bool PhonebookSearch::onInput(const InputEvent &inputEvent)
{
    // check if any of the lower inheritance onInput methods catch the event
    LOG_INFO("onInput state: %d code: %d", inputEvent.state, inputEvent.keyCode);
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

    if (inputEvent.keyCode == KeyCode::KEY_ENTER)
    {
        LOG_INFO("Enter pressed");
        std::string contents = inputField->getText();
        if (contents.size() > 122)
        {
            std::vector<std::string> result = utils::split(contents, ' ');
            std::unique_ptr<std::vector<ContactRecord>> contacts = DBServiceAPI::ContactSearchByName(
                application, result.size() >= 1 ? result[0] : "", result.size() == 2 ? result[1] : "");
            if (contacts)
            {
                LOG_INFO("search query returned %d records", contacts->size());
            }
        }
        else
        {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookSearchQuery>(inputField->getText());
            application->switchWindow("NoResults", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        }
        return (true);
    }
    else
    {
        LOG_INFO("onInput unhandled event code=%d", inputEvent.keyCode);
    }
    return (false);
}
