#pragma once

#include "OptionWindow.hpp"

#include <ContactRecord.hpp>

namespace gui
{
    class PhonebookNamecardOptions : public OptionWindow
    {
      public:
        PhonebookNamecardOptions(app::Application *app);
        ~PhonebookNamecardOptions() override = default;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        auto handleSwitchData(SwitchData *data) -> bool override;

      private:
        std::shared_ptr<ContactRecord> contact = nullptr;
        auto namecardOptionsList() -> std::list<gui::Option>;
        void sendViaSms();
        void sendViaBluetooth();
    };
}; // namespace gui
