// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <ContactRecord.hpp>
#include <Text.hpp>

namespace gui
{
    class PhonebookSearch : public AppWindow
    {
      public:
        explicit PhonebookSearch(app::ApplicationCommon *app);

      private:
        void buildInterface() override;
        auto handleSwitchData(SwitchData *data) -> bool override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto onInput(const InputEvent &inputEvent) -> bool override;

        std::shared_ptr<ContactRecord> contact = nullptr;
        Text *inputField                       = nullptr;
    };

} /* namespace gui */
