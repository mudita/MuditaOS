// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "presenter/BedsidePresenter.hpp"
#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;

    class BellSettingsBedsideWindow : public AppWindow, public app::bell_settings::BedsideWindowContract::View
    {
      public:
        static constexpr auto name = "BellSettingsBedside";

        BellSettingsBedsideWindow(app::ApplicationCommon *app,
                                  std::unique_ptr<app::bell_settings::BedsidePresenter> &&presenter);

        auto buildInterface() -> void override;
        auto onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) -> void override;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        auto onClose(CloseReason reason) -> void override;
        auto rebuild() -> void override;
        auto exit() -> void override;

      private:
        SideListView *listView{nullptr};
        std::unique_ptr<app::bell_settings::BedsidePresenter> presenter;
    };
} // namespace gui
