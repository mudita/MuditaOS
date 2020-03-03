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

PhonebookSearch::PhonebookSearch(app::Application *app) : AppWindow(app, "Search"), phonebookModel{new PhonebookModel(app)}
{
    buildInterface();
}

PhonebookSearch::~PhonebookSearch()
{
}

Label *PhonebookSearch::addLabel(std::list<Item *> *parentPage, int x, int y, int w, int h, const std::string text = "",
                                 const std::string fontName = style::window::font::small,
                                 const RectangleEdgeFlags edges = RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                                 const Alignment alignment = Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM),
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

    searchHeader = addLabel(nullptr, style::window::default_left_margin, 127, 86, 20, utils::localize.get("app_phonebook_search_win_search"));

    inputField = new Text(nullptr, 1, 1, style::window_width - style::window::default_left_margin - style::window::default_right_margin - 32, 33);
    inputField->setTextType(Text::TextType::SINGLE_LINE);
    inputField->setEditMode(Text::EditMode::EDIT);
    inputField->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    inputField->setInputMode(new InputMode({InputMode::ABC, InputMode::abc}));
    inputField->setFont(style::window::font::mediumbold);

    searchTop = new Image(nullptr, style::window_width - style::window::default_left_margin - style::window::default_right_margin, 1, 32, 32, "search");

    horizontalBox = new HBox(this, style::window::default_left_margin, 153, style::window_width - style::window::default_left_margin - style::window::default_right_margin, 33);
    horizontalBox->addWidget(inputField);
    horizontalBox->addWidget(searchTop);
    horizontalBox->setPenWidth(2);
    horizontalBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

    searchCenter = new Image(this, 224, 265, 32, 32, "phonebook_search_glass_grey");

    informationLabel = new Text(this, 64, 315, 352, 66);
    informationLabel->setText(utils::localize.get("app_phonebook_search_win_info"));
    informationLabel->setFillColor(ColorTray);
    informationLabel->setTextType(Text::TextType::MULTI_LINE);
    informationLabel->setEditMode(Text::EditMode::BROWSE);
    informationLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    informationLabel->setAlignment(Alignment::ALIGN_HORIZONTAL_CENTER);
    informationLabel->setFont(style::window::font::medium);
    informationLabel->setTextColor(style::color::lightgrey);

    fancyArrowDown = new Image(this, 180, 370, 54, 71, "phonebook_search_arrow_down_grey");

    bottomBar->setActive(BottomBar::Side::LEFT, false);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);

    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_search"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

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
}

bool PhonebookSearch::handleSwitchData(SwitchData *data)
{
    if (data == nullptr)
    {
        LOG_ERROR("Received null pointer");
        return false;
    }

    PhonebookSearchQuery *item = dynamic_cast<PhonebookSearchQuery *>(data);
    inputField->setText(item->getQuery());

    return (true);
}

bool PhonebookSearch::onInput(const InputEvent &inputEvent)
{
    bool ret = AppWindow::onInput(inputEvent);
    if(ret)
    {
        return ret;
    }

    if ((inputEvent.state != InputEvent::State::keyReleasedShort) && ((inputEvent.state != InputEvent::State::keyReleasedLong)))
    {
        return (false);
    }

    if (inputEvent.keyCode == KeyCode::KEY_ENTER)
    {
        LOG_INFO("Enter pressed");
        std::string contents = utils::trim(inputField->getText());
        std::shared_ptr<std::vector<ContactRecord>> searchResults;
        std::string primaryName = "";
        std::string alternativeName = "";
        std::string number = "";

        if (contents.size() > 0)
        {
            if (utils::is_number(contents) || contents.find('+') != std::string::npos)
            {
                number = contents;
            }
            else if (contents.find(' ') != std::string::npos)
            {
                std::vector<std::string> result = utils::split(contents, ' ');
                primaryName = result[0];
                alternativeName = result[1];
            }
            else
            {
                primaryName = contents;
            }

            LOG_INFO("searching for pri:%s alt:%s num:%s", primaryName.c_str(), alternativeName.c_str(), number.c_str());
            searchResults = DBServiceAPI::ContactSearch(application, primaryName, alternativeName, number);
            if (searchResults.get()->size() > 0)
            {
                std::unique_ptr<PhonebookSearchResultsData> data = std::make_unique<PhonebookSearchResultsData>(inputField->getText());
                data->setResults(searchResults);
                application->switchWindow("SearchResults", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            }
            else
            {
                std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookSearchQuery>(inputField->getText());
                application->switchWindow("NoResults", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            }
            return (true);
        }
    }

    return (ret);
}
