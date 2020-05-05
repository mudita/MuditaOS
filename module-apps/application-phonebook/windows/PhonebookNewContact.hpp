#pragma once

#include <memory>
#include <string>

#include "../data/PhonebookItemData.hpp"
#include "../Application.hpp"
#include "AppWindow.hpp"
#include "ContactRecord.hpp"
#include "Label.hpp"
#include "Style.hpp"
#include "Text.hpp"
#include <widgets/BoxLayout.hpp>

namespace gui
{
    namespace window
    {
        namespace name
        {
            inline std::string newContact = "New";
        }
    } // namespace window
    class PhonebookNewContact : public AppWindow
    {
      public:
        enum class PageLabels
        {
            Name,
            Surname,
            Number_1,
            Number_2,
            Email
        };

      protected:
        gui::Label *title = nullptr;

        struct Page
        {
            virtual void setVisible(bool visible) = 0;
        };

        struct Page1 : public Page
        {
            gui::Label *labels[5] = {nullptr};
            gui::Text *text[5]    = {nullptr};
            void setVisible(bool visible) override
            {
                for (uint32_t i = 0; i < 5; i++) {
                    labels[i]->setVisible(visible);
                    text[i]->setVisible(visible);
                }
            }
        };

        struct Page2 : public Page
        {
            gui::Label *speedValue       = nullptr;
            gui::Label *speedDescription = nullptr;
            gui::Image *imageSpeed       = nullptr;
            gui::Label *favValue         = nullptr;
            gui::Label *favDescription   = nullptr;
            gui::Image *imageFav         = nullptr;
            gui::Image *imageTick        = nullptr;

            gui::Label *addressLabel = nullptr;
            gui::Label *noteLabel    = nullptr;

            gui::Text *text[2] = {nullptr};

            bool favSelected = false;

            void setVisible(bool visible) override
            {
                speedValue->setVisible(visible);
                speedDescription->setVisible(visible);
                imageSpeed->setVisible(visible);
                favValue->setVisible(visible);
                favDescription->setVisible(visible);
                imageFav->setVisible(visible);
                addressLabel->setVisible(visible);
                noteLabel->setVisible(visible);
                text[0]->setVisible(visible);
                text[1]->setVisible(visible);
                imageTick->setVisible(visible && favSelected);
            }
        };

        Page1 page1;
        Page2 page2;
        uint32_t page = 0;

        std::shared_ptr<ContactRecord> contact = nullptr;

      public:
        PhonebookNewContact(app::Application *app);
        virtual ~PhonebookNewContact();
        void switchPage(uint32_t page);

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool handleSwitchData(SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        bool verifyAndSave();
        void copyInputData(ContactRecord &contactRecord);
        void setContactData();
        void saveStateChanged();
        const std::string getCountryPrefix();
    };

} /* namespace gui */
