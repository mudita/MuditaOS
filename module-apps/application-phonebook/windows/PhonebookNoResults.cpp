#include "PhonebookNoResults.hpp"
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

PhonebookNoResults::PhonebookNoResults(app::Application *app) : AppWindow(app, "NoResults")
{
    setSize(480, 600);
    buildInterface();
}

PhonebookNoResults::~PhonebookNoResults()
{
}

Label *PhonebookNoResults::addLabel(std::list<Item *> *parentPage, int x, int y, int w, int h,
                                    const std::string text = "", const std::string fontName = "gt_pressura_bold_24",
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

    if (parentPage)
        parentPage->push_back(l);

    return (l);
}

void PhonebookNoResults::buildInterface()
{
    AppWindow::buildInterface();
    topBar->setActive(TopBar::Elements::TIME, true);

    bottomBar->setActive(BottomBar::Side::LEFT, false);
    bottomBar->setActive(BottomBar::Side::CENTER, false);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);

    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    titleLabel = addLabel(nullptr, 0, 52, 480, 35, utils::localize.get("app_phonebook_search_win_contacts"),
                          "gt_pressura_bold_24", RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                          Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_BOTTOM));

    topSeparatorLabel = addLabel(nullptr, 0, 104, 480, 1, "", "", RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

    informationLabel = new Text(this, 45, 315, 390, 90);
    informationLabel->setText(utils::localize.get("app_phonebook_search_no_results"));
    informationLabel->setFillColor(ColorTray);
    informationLabel->setTextType(Text::TextType::MULTI_LINE);
    informationLabel->setEditMode(Text::EditMode::BROWSE);
    informationLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    informationLabel->setFont("gt_pressura_regular_24");

    searchCenter = new Image(this, 224, 265, 32, 32, "search");
}

void PhonebookNoResults::rebuild()
{
}

void PhonebookNoResults::destroyInterface()
{
    AppWindow::destroyInterface();
}

void PhonebookNoResults::onBeforeShow(ShowMode mode, SwitchData *data)
{
    application->setKeyboardProfile(utils::localize.get("common_kbd_upper"));
}

bool PhonebookNoResults::handleSwitchData(SwitchData *data)
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

void PhonebookNoResults::setContactData()
{
    titleLabel->setText(utils::localize.get("app_phonebook_search_results").c_str() + std::string(":\"") +
                        searchQuery.c_str() + std::string("\""));
}

bool PhonebookNoResults::onInput(const InputEvent &inputEvent)
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

    if ((inputEvent.state != InputEvent::State::keyReleasedShort) &&
        ((inputEvent.state != InputEvent::State::keyReleasedLong)))
        return false;

    if (inputEvent.keyCode == KeyCode::KEY_RF)
    {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookSearchQuery>(searchQuery);
        application->switchWindow("Search", 0, std::move(data));
        return (true);
    }
    else
    {
        LOG_INFO("unhandled event code=%d", inputEvent.keyCode);
    }
    return (false);
}
