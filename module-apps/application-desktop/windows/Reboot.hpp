// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>
#include "AppWindow.hpp"
#include <Text.hpp>
#include "../presenter/PowerOffPresenter.hpp"

namespace gui
{
    class RebootWindow : public AppWindow
    {
      public:
        RebootWindow(app::Application *app, std::unique_ptr<PowerOffPresenter> &&presenter);
        ~RebootWindow() override = default;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        void invalidate() noexcept;

        std::unique_ptr<PowerOffPresenter> presenter;
        Text *text = nullptr;
    };

} /* namespace gui */
