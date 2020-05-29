#pragma once

#include "ContactRecord.hpp"
#include "OptionWindow.hpp"

namespace gui
{
    class PhonebookNamecardOptions : public OptionWindow
    {
      public:
        PhonebookNamecardOptions(app::Application *app);
        ~PhonebookNamecardOptions() override;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        auto handleSwitchData(SwitchData *data) -> bool override;

      private:
        std::shared_ptr<ContactRecord> contact = nullptr;
        auto namecardOptionsList() -> std::list<gui::Option>;
        void sendViaSms();
        void sendViaBluetooth();
        auto formatVCARD() -> const std::string;
    };
}; // namespace gui
