// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <gui/widgets/Icon.hpp>

namespace gui
{
    class FactoryResetInProgressWindow : public AppWindow
    {
      private:
        Icon *infoIcon = nullptr;

      public:
        explicit FactoryResetInProgressWindow(app::ApplicationCommon *app);
        void buildInterface() override;
    };
} /* namespace gui */
