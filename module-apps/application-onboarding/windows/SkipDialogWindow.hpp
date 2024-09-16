// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Dialog.hpp"

namespace app::onBoarding
{
    class SkipDialogWindow : public gui::DialogYesNo
    {
      public:
        explicit SkipDialogWindow(app::ApplicationCommon *app);
    };
} // namespace app::onBoarding
