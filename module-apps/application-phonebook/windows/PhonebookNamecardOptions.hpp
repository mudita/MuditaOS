// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionWindow.hpp"

#include <ContactRecord.hpp>

namespace gui
{
    class PhonebookNamecardOptions : public OptionWindow
    {
      public:
        PhonebookNamecardOptions(app::ApplicationCommon *app);
        ~PhonebookNamecardOptions() override = default;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        auto handleSwitchData(SwitchData *data) -> bool override;

      private:
        std::shared_ptr<ContactRecord> contact = nullptr;
        auto namecardOptionsList() -> std::list<gui::Option>;
        auto sendViaSms() -> bool;
    };
}; // namespace gui
