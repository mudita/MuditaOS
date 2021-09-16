// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <OptionWindow.hpp>

namespace gui
{

    class BellMainMenuWindow : public OptionWindow
    {
      public:
        BellMainMenuWindow(app::ApplicationCommon *app);

      private:
        std::list<Option> mainMenuOptionsList();
        void buildInterface() override;
    };

} // namespace gui
