#pragma once

#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/models/ContactDetailsModel.hpp"

#include <Text.hpp>
#include <ListView.hpp>

namespace gui
{
    class ContactFlagsWidget;
    class PhonebookContactDetails : public AppWindow
    {
      public:
        PhonebookContactDetails(app::Application *app);
        ~PhonebookContactDetails() override;

        auto onInput(const InputEvent &inputEvent) -> bool override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto handleSwitchData(SwitchData *data) -> bool override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      protected:
        std::shared_ptr<ContactRecord> contact = nullptr;
        ContactFlagsWidget *contactFlagsWidget = nullptr;

      private:
        std::shared_ptr<ContactDetailsModel> contactDetailsModel = nullptr;
        gui::ListView *bodyList                                  = nullptr;
    };
} // namespace gui
