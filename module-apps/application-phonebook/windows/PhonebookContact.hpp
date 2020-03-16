#pragma once

#include "../data/PhonebookItemData.hpp"
#include "AppWindow.hpp"
#include "ContactRecord.hpp"
#include "Label.hpp"
#include "Style.hpp"
#include "Text.hpp"
#include <memory>
#include <string>
#include <widgets/BoxLayout.hpp>
#include <Utils.hpp>

static inline bool isValidName(const UTF8 name)
{
    return (name.length() > 0);
}

static inline bool isValidNumber(const UTF8 number)
{
    return (number.length() > 3);
}

static inline bool isValidEmail(const UTF8 email)
{
    return (email.length() > 4);
}

static inline const std::string formatContactName(std::shared_ptr<ContactRecord> contact)
{
    return (contact->primaryName + " " + contact->alternativeName);
}

static inline void fillContactData(std::string &data, std::shared_ptr<ContactRecord> contact)
{
    utils::findAndReplaceAll(data, "$CONTACT_PRIMARY_NAME$", contact->primaryName);
    utils::findAndReplaceAll(data, "$CONTACT_ALTERNATIVE_NAME$", contact->alternativeName);
    utils::findAndReplaceAll(data, "$CONTACT_NAME$", formatContactName(contact));
    utils::findAndReplaceAll(
        data, "$CONTACT_NUMBER1$", (contact->numbers.size() == 1) ? contact->numbers[0].numberE164 : "");
    utils::findAndReplaceAll(
        data, "$CONTACT_NUMBER2$", (contact->numbers.size() == 2) ? contact->numbers[1].numberE164 : "");
    utils::findAndReplaceAll(data, "$CONTACT_SPEED_DIAL$", std::to_string(contact->speeddial));
}

namespace gui
{
    namespace window
    {
        namespace name
        {
            inline std::string contact = "Contact";
        }
    } // namespace window

    class PhonebookContact : public AppWindow
    {
      protected:
        /** labels */
        Label *informationLabel = nullptr;
        Label *ifnormation      = nullptr;
        Label *addressLabel     = nullptr;
        Label *addressLine1     = nullptr;
        Label *addressLine2     = nullptr;
        Label *email            = nullptr;
        Label *numberPrimary    = nullptr;
        Label *numberSecondary  = nullptr;
        Label *favouritesLabel  = nullptr;
        Label *speeddialLabel   = nullptr;
        Label *blockedLabel     = nullptr;
        Label *speedDialLabel   = nullptr;
        Label *speedDial        = nullptr;
        Label *noteLabel        = nullptr;
        Text *noteText          = nullptr;

        /** icons */
        Image *favouritesIcon = nullptr;
        Image *blockedIcon    = nullptr;

        Label *numberPrimaryLabel = nullptr;
        Image *numberPrimaryIcon  = nullptr;

        Label *numberPrimaryMessageLabel = nullptr;
        Image *numberPrimaryMessage      = nullptr;

        Label *numberSecondaryLabel = nullptr;
        Image *numberSecondaryIcon  = nullptr;

        Label *numberSecondaryMessageLabel = nullptr;
        Image *numberSecondaryMessage      = nullptr;

        ContactRecord readContact();

        /** currently displayed contact */
        std::shared_ptr<ContactRecord> contact = nullptr;

        std::list<Item *> page1;
        std::list<Item *> page2;

        void setVisible(std::list<Item *> *page, bool shouldBeVisible);

      public:
        PhonebookContact(app::Application *app);
        virtual ~PhonebookContact();

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool handleSwitchData(SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        void setContactData();
        Label *addLabel(std::list<Item *> *parentPage,
                        int x,
                        int y,
                        int w,
                        int h,
                        const std::string text,
                        const std::string fontName,
                        const RectangleEdgeFlags edges,
                        const Alignment alignment,
                        const bool lineMode);
    };
} // namespace gui
