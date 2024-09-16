// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/windows/UpdateInstructionWindowContract.hpp>
#include <apps-common/ApplicationCommon.hpp>

#include <vector>
#include <Item.hpp>

namespace app::bell_settings
{
    class UpdateInstructionWindowPresenter : public gui::UpdateInstructionWindowContract::Presenter
    {
      private:
        app::ApplicationCommon *app;
        std::vector<gui::Item *> layoutOptions;
        void initLayoutOptions();

      public:
        explicit UpdateInstructionWindowPresenter(app::ApplicationCommon *app);

        std::vector<gui::Item *> getLayouts() const override;
        bool isLastLayout(const gui::Item *layout) const override;
        gui::Item *getFirstLayout() const override;
        bool onInput(const gui::InputEvent &inputEvent, const gui::Item *currentLayout) override;
    };
} // namespace app::bell_settings
