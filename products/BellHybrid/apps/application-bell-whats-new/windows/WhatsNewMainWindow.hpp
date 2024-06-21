// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "WhatsNewCommon.hpp"
#include "WhatsNewMainPresenter.hpp"
#include <common/options/BellOptionWithDescriptionWindow.hpp>

namespace app::whatsnew
{
    using namespace gui;

    class WhatsNewMainWindow : public BellOptionWithDescriptionWindow
    {
      public:
        WhatsNewMainWindow(app::ApplicationCommon *app,
                           std::unique_ptr<WhatsNewMainContract::Presenter> &&presenter,
                           const std::string &name = window::name::main);
        auto onBeforeShow(ShowMode mode, SwitchData *data) -> void override;

      private:
        std::unique_ptr<WhatsNewMainContract::Presenter> presenter;

        auto settingsOptionsList() -> std::list<Option>;
    };
} // namespace app::whatsnew
