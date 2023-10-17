// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/presenter/PowerOffPresenter.hpp>
#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class Icon;

    class BellFactoryReset : public WindowWithTimer
    {
      public:
        static constexpr auto name = "BellFactoryResetWindow";

        BellFactoryReset(app::ApplicationCommon *app, std::unique_ptr<AbstractPowerOffPresenter> presenter);

      private:
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;

        Icon *icon{};
        std::unique_ptr<AbstractPowerOffPresenter> presenter;
    };

} // namespace gui
