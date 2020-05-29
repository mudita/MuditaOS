#pragma once

#include "ContactRecord.hpp"
#include "OptionWindow.hpp"

namespace gui
{
    class PhonebookContactOptions : public OptionWindow
    {
      public:
        PhonebookContactOptions(app::Application *app);
        ~PhonebookContactOptions() override;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        auto handleSwitchData(SwitchData *data) -> bool override;

      private:
        std::shared_ptr<ContactRecord> contact = nullptr;
        auto contactOptionsList() -> std::list<gui::Option>;
        auto contactBlock() -> bool;
        auto contactRemove() -> bool;
    };
}; // namespace gui
