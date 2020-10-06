#pragma once

#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/models/ContactDetailsModel.hpp"

#include <AppWindow.hpp>
#include <ListView.hpp>
#include <Text.hpp>

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

      private:
        std::shared_ptr<ContactRecord> contact                   = nullptr;
        ContactFlagsWidget *contactFlagsWidget                   = nullptr;
        std::shared_ptr<ContactDetailsModel> contactDetailsModel = nullptr;
        gui::ListView *bodyList                                  = nullptr;
    };
} // namespace gui
