// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionWindow.hpp"

namespace gui
{
    class AllDevicesWindow : public OptionWindow
    {
      public:
        AllDevicesWindow(app::Application *app);
        auto onInput(const InputEvent &inputEvent) -> bool override;

      private:
        Image *leftArrowImage = nullptr;
        Image *crossImage     = nullptr;
        auto allDevicesOptionsList() -> std::list<Option>;
    };
}; // namespace gui
