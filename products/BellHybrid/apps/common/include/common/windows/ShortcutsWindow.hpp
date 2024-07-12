// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ShortcutsWindowContract.hpp"
#include <AppWindow.hpp>
#include <apps-common/widgets/spinners/Spinners.hpp>

namespace gui
{
    class SideListView;

    class ShortcutsWindow : public AppWindow, public ShortcutsWindowContract::View
    {
      public:
        ShortcutsWindow(app::ApplicationCommon *app,
                        std::unique_ptr<ShortcutsWindowContract::Presenter> &&presenter,
                        const std::string &name);

        [[nodiscard]] bool isOneOfTwoLastShortcuts() const;

      private:
        std::unique_ptr<ShortcutsWindowContract::Presenter> presenter;
        WidgetSpinner *spinner{nullptr};

        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;
    };
} // namespace gui
