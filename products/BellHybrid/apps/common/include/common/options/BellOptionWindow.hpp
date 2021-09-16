// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ApplicationCommon.hpp>
#include <AppWindow.hpp>
#include <OptionsList.hpp>
#include <ThreeBox.hpp>

namespace style::bell_options_list
{
    inline constexpr auto w                    = 380U;
    inline constexpr auto center_layout_h      = 400U;
    inline constexpr auto outer_layouts_h      = 64U;
    inline constexpr auto outer_layouts_margin = 7U;
} // namespace style::bell_options_list

namespace gui
{
    class BellOptionWindow : public AppWindow, protected OptionsList
    {
      public:
        BellOptionWindow(app::ApplicationCommon *app, const std::string &name);

        VThreeBox<VBox, VBox, VBox> *body = nullptr;
        void setListTitle(const std::string &title);
        void createLayout();

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        void rebuild() override;
        void buildInterface() override;
    };
}; // namespace gui
