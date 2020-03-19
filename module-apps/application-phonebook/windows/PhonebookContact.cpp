#include "PhonebookContact.hpp"
#include "../ApplicationPhonebook.hpp"
#include "InputEvent.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "PhonebookNewContact.hpp"
#include "Text.hpp"
#include "UiCommonActions.hpp"
#include "Utils.hpp"
#include "application-call/ApplicationCall.hpp"
#include "application-messages/ApplicationMessages.hpp"
#include "application-messages/data/SMSdata.hpp"
#include "application-messages/windows/ThreadViewWindow.hpp"
#include "i18/i18.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include <log/log.hpp>

namespace gui
{

    PhonebookContact::PhonebookContact(app::Application *app) : AppWindow(app, window::name::contact)
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
        l->setAlignement(alignment);
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
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("common_options"));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_call"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

        favouritesIcon  = new Image(this, 97, 107, 32, 32, "small_heart");
        favouritesLabel = addLabel(nullptr,
                                   65,
                                   144,
                                   89,
                                   20,
                                   utils::localize.get("app_phonebook_contact_favourites_upper"),
                                   style::window::font::smallbold,
                                   RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                                   Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));

        speedDial = addLabel(nullptr,
                             225,
                             107,
                             32,
                             32,
                             "",
                             style::footer::font::bold,
                             RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                             Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));

        speedDialLabel = addLabel(nullptr,
                                  196,
                                  144,
                                  89,
                                  20,
                                  utils::localize.get("app_phonebook_contact_speed_dial_upper"),
                                  style::window::font::smallbold,
                                  RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                                  Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));

        blockedIcon  = new Image(this, 351, 107, 32, 32, "small_circle");
        blockedLabel = addLabel(nullptr,
                                329,
                                144,
                                75,
                                20,
                                utils::localize.get("app_phonebook_contact_speed_blocked_uppper"),
                                style::window::font::smallbold,
                                RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                                Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));

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
                return app::call(application, app::CallOperation::ExecuteCall, contact->numbers[0].numberE164);
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
        numberPrimaryMessageLabel->inputCallback = [=](Item &item, const InputEvent &input) {
            if (input.keyCode == KeyCode::KEY_ENTER) {
                LOG_DEBUG("numberPrimayLabel->inputCallback switch to sms window");
                return sapm::ApplicationManager::messageSwitchApplication(application,
                                                                          app::name_messages,
                                                                          gui::name::window::thread_view,
                                                                          std::make_unique<SMSSendRequest>(contact));
            }
            LOG_DEBUG("numberPrimaryMessageLabel->inputCallback just return false");
            return (false);
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
                return app::call(application, app::CallOperation::ExecuteCall, contact->numbers[1].numberE164);
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
        numberSecondaryMessageLabel->inputCallback = [=](Item &item, const InputEvent &input) {
            if (input.keyCode == KeyCode::KEY_ENTER) {
                return sapm::ApplicationManager::messageSwitchApplication(application,
                                                                          app::name_messages,
                                                                          gui::name::window::thread_view,
                                                                          std::make_unique<SMSSendRequest>(contact));
            }
            return (false);
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

        addressLine1 = addLabel(&page1,
                                30,
                                475,
                                422,
                                33,
                                "",
                                style::window::font::big,
                                RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES,
                                Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_CENTER));

        addressLine2 = addLabel(&page1,
                                30,
                                508,
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
        AppWindow::destroyInterface();
        children.clear();
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

        if (contact && contact->primaryName.length() > 0)
            setTitle(contact->primaryName);

        if (contact && contact->primaryName.length() > 0 && contact->alternativeName.length() > 0)
            setTitle(contact->primaryName + " " + contact->alternativeName);

        if (contact->speeddial >= 0 && contact->speeddial < 10) {
            speedDial->setText(std::to_string(contact->speeddial));
        }
        else {
            speedDial->setText(UTF8("-"));
        }

        if (contact->isOnFavourites == false) {
            LOG_INFO("setContactData contact %s is not on fav list", contact->primaryName.c_str());
            favouritesIcon->setVisible(false);
            favouritesLabel->setVisible(false);
        }
        else {
            LOG_INFO("setContactData contact %s is on fav list", contact->primaryName.c_str());
        }

        if (contact->isOnBlacklist == false) {
            blockedLabel->setVisible(false);
            blockedIcon->setVisible(false);
        }

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
            addressLine1->setY(475 - 33);
            addressLine2->setY(508 - 33);
        }

        if (contact->numbers.size() == 1) {
            numberPrimary->setText(contact->numbers[0].numberE164);

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
            addressLine1->setY(475 - 33);
            addressLine2->setY(508 - 33);
        }

        if (contact->numbers.size() == 2) {

            numberPrimary->setText(contact->numbers[0].numberE164);
            numberSecondary->setText(contact->numbers[1].numberE164);
            email->setY(363);
            addressLabel->setY(429);
            addressLine1->setY(475);
            addressLine2->setY(508);
        }

        if (contact->mail.length() > 0) {
            email->setText(contact->mail);
        }
        else {
            email->setVisible(false);
        }

        if (contact->street.length() > 0) {
            addressLine1->setText(contact->street);
        }

        if (contact->city.length() > 0) {
            addressLine2->setText(contact->city);
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
        if ((inputEvent.state != InputEvent::State::keyReleasedShort) &&
            ((inputEvent.state != InputEvent::State::keyReleasedLong)) && inputEvent.keyCode == KeyCode::KEY_LF) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return (true);
        }

        return (AppWindow::onInput(inputEvent));
    }

} // namespace gui