// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MmiPushWindow.hpp"

#include <AppWindow.hpp>
#include <DialogMetadata.hpp>
#include <service-appmgr/data/MmiActionsParams.hpp>
#include <Text.hpp>

#include <functional>
#include <map>

namespace gui
{
    class MmiInternalMsgWindow : public MmiPushWindow
    {
      public:
        MmiInternalMsgWindow(app::ApplicationCommon *app, const std::string &name);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void handleInternalMessages(mmiactions::MMIResultParams *metadata);
    };

}; // namespace gui
