// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include "Application.hpp"
#include <module-bsp/bsp/cellular/bsp_cellular.hpp>
#include <module-gui/gui/widgets/Text.hpp>

namespace gui
{

    namespace window
    {
        namespace cellular_passthrough
        {
            inline constexpr auto window_name = "CellularPassthroughWindow";

            inline constexpr auto passNormalDescription = "Regular passthrough";
            inline constexpr auto passNormalLabel       = "ON";

            inline constexpr auto passDFUDescription = "Firmware Upgrade";
            inline constexpr auto passDFULabel       = "ON (upgrade)";
            inline constexpr auto passDFUNote        = "Reset modem with 2*PWR_KEY";

            inline constexpr auto noPassDescription = "Disable passthrough";
            inline constexpr auto noPassLabel       = "OFF";
            inline constexpr auto noPassNote        = "Reset modem hard. (battery out)";
        } // namespace cellular_passthrough
    }     // namespace window

    class CellularPassthroughWindow : public AppWindow
    {
      private:
        VBox *layout = nullptr;

        Label *passNormal = nullptr;
        Label *passDFU    = nullptr;
        Label *noPass     = nullptr;

        HBox *statusHbox            = nullptr;
        Label *currentStateConst    = nullptr;
        Label *currentStateVariable = nullptr;

        Label *note = nullptr;

      protected:
        enum class State
        {
            PASS_NORMAL,
            PASS_DFU,
            NO_PASS,
            FAIL,
            UNKNOWN,
        };

        State previousState = State::UNKNOWN;

        const time_t wait_for_pin_change_state = 10; // [ms]

      public:
        CellularPassthroughWindow(app::Application *app);

        virtual ~CellularPassthroughWindow() override = default;

        void rebuild() override;

        void buildInterface() override;

        void destroyInterface() override;

        void addSeparator(VBox *layout);

        CellularPassthroughWindow::State getInitialState();

        bool set(bsp::cellular::USB::PassthroughState pass_to_set, bsp::cellular::USB::BootPinState dfu_to_set);

        void setWindowState(State state);

      private:
        void invalidate() noexcept;
    };
} // namespace gui
