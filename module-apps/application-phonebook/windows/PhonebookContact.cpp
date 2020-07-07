#include "PhonebookContact.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookInternals.hpp"
#include "application-phonebook/widgets/ContactFlagsWidget.hpp"
#include "UiCommonActions.hpp"

namespace gui
{

    PhonebookContact::PhonebookContact(app::Application *app) : AppWindow(app, gui::window::name::contact)
    {
        buildInterface();
    }

    void PhonebookContact::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    Label *PhonebookContact::addLabel(std::list<Item *> *parentPage,
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
        Label *l = new Label(this, x, y, w, h);
        l->setFilled(false);
        l->setBorderColor(ColorFullBlack);
        l->setEdges(edges);
        l->setFont(fontName);
        l->setText(text);
        l->setAlignment(alignment);
        l->setLineMode(lineMode);

        if (parentPage)
            parentPage->push_back(l);

        return (l);
    }

    void PhonebookContact::buildInterface()
    {
        AppWindow::buildInterface();
        topBar->setActive(TopBar::Elements::TIME, true);

        bottomBar->setActive(BottomBar::Side::LEFT, true);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(style::strings::common::options));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_call"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

        title->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        contactFlagsWidget = new ContactFlagsWidget(this);
        contactFlagsWidget->setVisible(true);

        // page1 contents
        informationLabel = addLabel(&page1,
                                    30,
                                    203,
                                    413,
                                    20,
                                    utils::localize.get("app_phonebook_contact_information"),
                                    style::window::font::small,
                                    RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                                    Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM),
                                    true);

        // first number line
        numberPrimary = addLabel(&page1,
                                 30,
                                 249,
                                 178,
                                 33,
                                 "",
                                 style::window::font::bigbold,
                                 RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                                 Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));

        numberPrimaryLabel                = addLabel(&page1,
                                      328,
                                      237,
                                      32,
                                      53,
                                      "",
                                      style::window::font::small,
                                      RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP,
                                      Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
        numberPrimaryLabel->inputCallback = [=](Item &item, const InputEvent &input) {
            if (input.keyCode == KeyCode::KEY_ENTER && input.state == InputEvent::State::keyReleasedShort) {
                return app::call(application, contact->numbers[0].number);
            }
            LOG_DEBUG("numberPrimayLabel->inputCallback just return false");
            return (false);
        };

        numberPrimaryLabel->setPenFocusWidth(3);
        numberPrimaryLabel->setPenWidth(0);

        numberPrimaryIcon = new Image(this, 328, 249, 32, 32, "phonebook_phone_ringing");
        page1.push_back(numberPrimaryIcon);

        numberPrimaryMessageLabel =
            addLabel(&page1,
                     401,
                     237,
                     32,
                     53,
                     "",
                     style::window::font::small,
                     RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP,
                     Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));

        numberPrimaryMessageLabel->setPenFocusWidth(3);
        numberPrimaryMessageLabel->setPenWidth(0);
        numberPrimaryMessageLabel->activatedCallback = [=](Item &item) {
            return app::sms(application, app::SmsOperation::New, contact->numbers[0].number);
        };

        numberPrimaryMessage = new Image(this, 401, 248, 32, 32, "mail");
        page1.push_back(numberPrimaryMessage);

        // second number line
        numberSecondary = addLabel(&page1,
                                   30,
                                   306,
                                   178,
                                   33,
                                   "",
                                   style::window::font::big,
                                   RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                                   Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));

        numberSecondaryLabel =
            addLabel(&page1,
                     328,
                     297,
                     32,
                     53,
                     "",
                     style::window::font::small,
                     RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP,
                     Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
        numberSecondaryLabel->setPenFocusWidth(3);
        numberSecondaryLabel->setPenWidth(0);
        numberSecondaryLabel->inputCallback = [=](Item &item, const InputEvent &input) {
            if (input.keyCode == KeyCode::KEY_ENTER) {
                return app::call(application, contact->numbers[1].number);
            }
            return (false);
        };

        numberSecondaryIcon = new Image(this, 328, 308, 32, 32, "phonebook_phone_ringing");
        page1.push_back(numberSecondaryIcon);

        numberSecondaryMessageLabel =
            addLabel(&page1,
                     401,
                     297,
                     32,
                     53,
                     "",
                     style::window::font::small,
                     RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP,
                     Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
        numberSecondaryMessageLabel->setPenFocusWidth(3);
        numberSecondaryMessageLabel->setPenWidth(0);
        numberSecondaryMessageLabel->activatedCallback = [=](Item &item) {
            return app::sms(application, app::SmsOperation::New, contact->numbers[1].number);
        };

        numberSecondaryMessage = new Image(this, 401, 306, 32, 32, "mail");
        page1.push_back(numberSecondaryMessage);
        email = addLabel(&page1,
                         30,
                         363,
                         413,
                         33,
                         "",
                         style::window::font::big,
                         RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                         Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_CENTER));

        addressLabel = addLabel(&page1,
                                30,
                                429,
                                413,
                                20,
                                utils::localize.get("app_phonebook_contact_address"),
                                style::window::font::small,
                                RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                                Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM),
                                true);

        address = addLabel(&page1,
                           30,
                           475,
                           422,
                           33,
                           "",
                           style::window::font::big,
                           RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                           Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_CENTER));

        noteLabel = addLabel(&page2,
                             30,
                             203,
                             413,
                             20,
                             utils::localize.get("app_phonebook_contact_note"),
                             style::window::font::small,
                             RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                             Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM),
                             true);

        noteText = new Text(this, 30, 249, 422, 600 - 249 - bottomBar->getHeight());
        noteText->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        noteText->setFont(style::window::font::small);
        page2.push_back(noteText);
        noteText->focusChangedCallback = [=](gui::Item &item) {
            setVisible(&page2, item.focus);
            setVisible(&page1, !item.focus);
            setContactData();
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            return (true);
        };

        // naviagation
        numberPrimaryLabel->setNavigationItem(NavigationDirection::DOWN, numberPrimaryMessageLabel);
        numberPrimaryMessageLabel->setNavigationItem(NavigationDirection::UP, numberPrimaryLabel);
        numberPrimaryMessageLabel->setNavigationItem(NavigationDirection::DOWN, numberSecondaryLabel);
        numberSecondaryLabel->setNavigationItem(NavigationDirection::UP, numberPrimaryMessageLabel);
        numberSecondaryLabel->setNavigationItem(NavigationDirection::DOWN, numberSecondaryMessageLabel);
        numberSecondaryMessageLabel->setNavigationItem(NavigationDirection::UP, numberSecondaryLabel);
        numberSecondaryMessageLabel->setNavigationItem(NavigationDirection::DOWN, noteText);
        noteText->setNavigationItem(NavigationDirection::UP, numberSecondaryMessageLabel);
        setFocusItem(numberPrimaryLabel);
        setVisible(&page2, false);
    }

    void PhonebookContact::setVisible(std::list<Item *> *page, bool shouldBeVisible)
    {
        for (auto i : *page) {
            i->setVisible(shouldBeVisible);
        }
    }

    void PhonebookContact::destroyInterface()
    {
        erase();
        // data was erased, now we need to clear page so that we won't use old pointers
        page1.clear();
        page2.clear();
    }

    PhonebookContact::~PhonebookContact()
    {
        destroyInterface();
    }

    void PhonebookContact::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

    ContactRecord PhonebookContact::readContact()
    {
        ContactRecord ret;
        return ret;
    }

    void PhonebookContact::setContactData()
    {
        if (contact == nullptr)
            return;

        setTitle(contact->getFormattedName(ContactRecord::NameFormatType::Title));

        if (contact->numbers.size() == 0) {
            numberPrimary->setVisible(false);
            numberPrimaryLabel->setVisible(false);
            numberPrimaryIcon->setVisible(false);
            numberPrimaryMessage->setVisible(false);
            numberPrimaryMessageLabel->setVisible(false);

            numberSecondary->setVisible(false);
            numberSecondaryLabel->setVisible(false);
            numberSecondaryIcon->setVisible(false);
            numberSecondaryMessage->setVisible(false);
            numberSecondaryMessageLabel->setVisible(false);

            email->setY(363 - 66);
            addressLabel->setY(429 - 33);
            address->setY(475 - 33);
        }

        if (contact->numbers.size() == 1) {
            numberPrimary->setText(contact->numbers[0].number.getFormatted());

            numberSecondary->setVisible(false);
            numberSecondaryLabel->setVisible(false);
            numberSecondaryIcon->setVisible(false);
            numberSecondaryMessage->setVisible(false);
            numberSecondaryMessageLabel->setVisible(false);

            numberPrimaryLabel->setNavigationItem(NavigationDirection::DOWN, numberPrimaryMessageLabel);
            numberPrimaryMessageLabel->setNavigationItem(NavigationDirection::UP, numberPrimaryLabel);
            numberPrimaryMessageLabel->setNavigationItem(NavigationDirection::DOWN, noteText);
            noteText->setNavigationItem(NavigationDirection::UP, numberPrimaryMessageLabel);

            email->setY(363 - 33);
            addressLabel->setY(429 - 33);
            address->setY(475 - 33);
        }

        if (contact->numbers.size() == 2) {

            numberPrimary->setText(contact->numbers[0].number.getFormatted());
            numberSecondary->setText(contact->numbers[1].number.getFormatted());
            email->setY(363);
            addressLabel->setY(429);
            address->setY(475);
        }

        if (contact->mail.length() > 0) {
            email->setText(contact->mail);
        }
        else {
            email->setVisible(false);
        }

        if (contact->address.length() > 0) {
            address->setText(contact->address);
        }

        if (contact->note.length() > 0) {
            noteText->setText(contact->note);
        }
    }

    bool PhonebookContact::handleSwitchData(SwitchData *data)
    {
        if (data == nullptr) {
            LOG_ERROR("Received null pointer");
            return false;
        }

        PhonebookItemData *item = dynamic_cast<PhonebookItemData *>(data);
        contact                 = item->getContact();

        setContactData();

        return (true);
    }

    bool PhonebookContact::onInput(const InputEvent &inputEvent)
    {
        // process only if key is released
        if ((inputEvent.state == InputEvent::State::keyReleasedShort) && (inputEvent.keyCode == KeyCode::KEY_LF)) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow(
                gui::window::name::contact_options, gui::ShowMode::GUI_SHOW_INIT, std::move(data));

            return true;
        }

        if (inputEvent.keyCode == KeyCode::KEY_RF) {
            contact = nullptr;
            rebuild();
        }
        // used for testing will be removed with finished model
        if (inputEvent.isShortPress()) {
            if (inputEvent.is(KeyCode::KEY_1)) {
                contactFlagsWidget->setFavourites(!contactFlagsWidget->getFavourites());
                getApplication()->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                return true;
            }

            if (inputEvent.is(KeyCode::KEY_2)) {
                contactFlagsWidget->setICE(!contactFlagsWidget->getICE());
                getApplication()->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                return true;
            }

            if (inputEvent.is(KeyCode::KEY_3)) {
                static unsigned char speeDialPosition = 0;
                if (contactFlagsWidget->getSpeedDial()) {
                    ++speeDialPosition %= 10;
                }
                contactFlagsWidget->setSpeedDial(!contactFlagsWidget->getSpeedDial(), speeDialPosition);
                getApplication()->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                return true;
            }

            if (inputEvent.is(KeyCode::KEY_4)) {
                contactFlagsWidget->setBlocked(!contactFlagsWidget->getBlocked());
                getApplication()->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                return true;
            }
        }

        return (AppWindow::onInput(inputEvent));
    }

} // namespace gui
