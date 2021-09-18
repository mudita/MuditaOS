// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>

namespace gui
{
    class BGSoundsPausedWindow : public AppWindow
    {
        void buildInterface() override;
        auto onInput(const InputEvent &inputEvent) -> bool override;

      public:
        explicit BGSoundsPausedWindow(app::ApplicationCommon *app);
    };

} // namespace gui
