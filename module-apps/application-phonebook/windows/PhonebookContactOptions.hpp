#pragma once

#include "OptionWindow.hpp"

#include <ContactRecord.hpp>

namespace gui
{
    class PhonebookContactOptions : public OptionWindow
    {
      public:
        PhonebookContactOptions(app::Application *app);
        ~PhonebookContactOptions() override = default;
        auto handleSwitchData(SwitchData *data) -> bool override;

      private:
        std::shared_ptr<ContactRecord> contact = nullptr;
        auto contactOptionsList() -> std::list<gui::Option>;
        auto contactBlock() -> bool;
        auto contactRemove() -> bool;
    };
}; // namespace gui
