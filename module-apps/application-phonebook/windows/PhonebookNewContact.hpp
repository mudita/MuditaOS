#pragma once

#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/models/NewContactModel.hpp"

#include <Text.hpp>
#include <ListView.hpp>

namespace gui
{
    class PhonebookNewContact : public AppWindow
    {
      public:
        PhonebookNewContact(app::Application *app);

        auto onInput(const InputEvent &inputEvent) -> bool override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto handleSwitchData(SwitchData *data) -> bool override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      protected:
        std::shared_ptr<ContactRecord> contact = nullptr;

      private:
        enum class ContactAction
        {
            None,
            Add,
            Edit
        };

        auto verifyAndSave() -> bool;
        void showDialogDuplicatedNumber(const utils::PhoneNumber::View &duplicatedNumber);
        void showDialogDuplicatedSpeedDialNumber();

        std::shared_ptr<NewContactModel> newContactModel = nullptr;
        gui::ListView *list                              = nullptr;
        ContactAction contactAction                      = ContactAction::None;
    };

} /* namespace gui */
