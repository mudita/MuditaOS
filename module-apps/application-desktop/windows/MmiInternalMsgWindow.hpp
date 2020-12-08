// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MmiPushWindow.hpp"

#include <AppWindow.hpp>
#include <Text.hpp>
#include <functional>
#include <DialogMetadata.hpp>

namespace gui
{
    class MmiInternalMsgWindow : public MmiPushWindow
    {
      public:
        MmiInternalMsgWindow(app::Application *app, const std::string &name);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };

}; // namespace gui
