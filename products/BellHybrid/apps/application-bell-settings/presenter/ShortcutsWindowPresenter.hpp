// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/windows/ShortcutsWindowContract.hpp>
#include <apps-common/ApplicationCommon.hpp>

#include <vector>
#include <Item.hpp>

namespace app::bell_settings
{
    class ShortcutsWindowPresenter : public gui::ShortcutsWindowContract::Presenter
    {
      private:
        std::vector<gui::Item *> layoutOptions;
        void initLayoutOptions();

      public:
        explicit ShortcutsWindowPresenter(app::ApplicationCommon *app);

        std::vector<gui::Item *> getLayouts() const override;
        bool isLastLayout(const gui::Item *layout) const override;
        bool isOneOfTwoLastLayouts(const gui::Item *layout) const override;
        gui::Item *getFirstLayout() const override;
        bool onInput(const gui::InputEvent &inputEvent, const gui::Item *currentLayout) override;
    };
} // namespace app::bell_settings
