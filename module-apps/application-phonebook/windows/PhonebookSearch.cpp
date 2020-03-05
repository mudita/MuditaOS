#include "PhonebookSearch.hpp"
#include "../ApplicationPhonebook.hpp"
#include "InputEvent.hpp"
#include "Label.hpp"
#include "PhonebookContact.hpp"
#include "../data/PhonebookStyle.hpp"
#include "Text.hpp"
#include "Utils.hpp"
#include "application-call/data/CallSwitchData.hpp"
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

    if (parentPage)
        parentPage->push_back(l);

    return (l);
}

void PhonebookSearch::buildInterface()
{
    AppWindow::buildInterface();
    topBar->setActive(TopBar::Elements::TIME, true);

    setTitle(utils::localize.get("app_phonebook_title_main"));

    searchHeader = addLabel(nullptr, phonebookStyle::search::searchHeader::x, phonebookStyle::search::searchHeader::y, phonebookStyle::search::searchHeader::w, phonebookStyle::search::searchHeader::h, utils::localize.get("app_phonebook_search_win_search"));
    searchHeader->setFont(style::window::font::small);

    inputField = new Text(nullptr, phonebookStyle::search::horizontalBox::inputField::x, phonebookStyle::search::horizontalBox::inputField::y, phonebookStyle::search::horizontalBox::inputField::w, phonebookStyle::search::horizontalBox::inputField::h);
    inputField->setTextType(Text::TextType::SINGLE_LINE);
    inputField->setEditMode(Text::EditMode::EDIT);
    inputField->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    inputField->setInputMode(new InputMode({InputMode::ABC, InputMode::abc}));
    inputField->setFont(style::window::font::mediumbold);

    searchTop = new Image(nullptr, phonebookStyle::search::horizontalBox::searchTop::x, phonebookStyle::search::horizontalBox::searchTop::y, phonebookStyle::search::horizontalBox::searchTop::w, phonebookStyle::search::horizontalBox::searchTop::h, "search");

    horizontalBox = new HBox(this, phonebookStyle::search::horizontalBox::x, phonebookStyle::search::horizontalBox::y, phonebookStyle::search::horizontalBox::w, phonebookStyle::search::horizontalBox::h);
    horizontalBox->addWidget(inputField);
    horizontalBox->addWidget(searchTop);
    horizontalBox->setPenWidth(phonebookStyle::search::horizontalBox::penWidth);
    horizontalBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

    bottomBar->setActive(BottomBar::Side::LEFT, false);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);

    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_search"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

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
