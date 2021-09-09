// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/windows/OptionWindow.hpp>

namespace gui
{

    class BellSettingsWindow : public OptionWindow
    {
      public:
        explicit BellSettingsWindow(app::ApplicationCommon *app);

      private:
        std::list<Option> settingsOptionsList();
        void buildInterface() override;
    };

} // namespace gui
