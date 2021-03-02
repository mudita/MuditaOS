// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>
#include <module-apps/windows/Dialog.hpp>
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "../presenter/PowerOffPresenter.hpp"

namespace gui
{
    class PowerOffWindow : public DialogYesNo
    {

        std::unique_ptr<PowerOffPresenter> presenter;
      public:
        PowerOffWindow(app::Application *app, std::unique_ptr<PowerOffPresenter> &&presenter);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        top_bar::Configuration configureTopBar(top_bar::Configuration appConfiguration) override;
    };

} /* namespace gui */
