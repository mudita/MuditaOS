// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <windows/Dialog.hpp>
#include <popups/presenter/PowerOffPresenter.hpp>

namespace gui
{
    class PowerOffWindow : public DialogYesNo
    {
        std::unique_ptr<PowerOffPresenter> presenter;

      public:
        PowerOffWindow(app::ApplicationCommon *app, std::unique_ptr<PowerOffPresenter> &&presenter);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };

} /* namespace gui */
