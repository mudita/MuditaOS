// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ShortcutsWindowContract.hpp"
#include <ApplicationCommon.hpp>
#include <AppWindow.hpp>
#include <apps-common/widgets/spinners/Spinners.hpp>

namespace gui
{
    class SideListView;
    class ShortcutsWindow : public AppWindow, public ShortcutsWindowContract::View
    {
        std::unique_ptr<ShortcutsWindowContract::Presenter> presenter;
        SideListView *sideListView = nullptr;
        WidgetSpinner *spinner     = nullptr;

        bool onInput(const gui::InputEvent &inputEvent) override;
        void buildInterface() override;

        void onValueChanged(const std::uint32_t currentValue);

      public:
        ShortcutsWindow(app::ApplicationCommon *app,
                        std::unique_ptr<ShortcutsWindowContract::Presenter> &&presenter,
                        const std::string &name);

        bool isOneOfTwoLastShortcuts() const;
    };
} // namespace gui
