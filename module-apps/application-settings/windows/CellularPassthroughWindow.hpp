#pragma once

#include <module-gui/gui/widgets/Text.hpp>
#include <module-bsp/bsp/cellular/bsp_cellular.hpp>
#include "Application.hpp"

namespace gui
{

    namespace window
    {
        namespace cellular_passthrough
        {
            inline const std::string window_name = "CellularPassthroughWindow";

            inline const UTF8 passNormalDescription = "Regular passthrough";
            inline const UTF8 passNormalLabel       = "ON";

            inline const UTF8 passDFUDescription = "Firmware Upgrade";
            inline const UTF8 passDFULabel       = "ON (upgrade)";
            inline const UTF8 passDFUNote        = "Reset modem with 2*PWR_KEY";

            inline const UTF8 noPassDescription = "Disable passthrough";
            inline const UTF8 noPassLabel       = "OFF";
            inline const UTF8 noPassNote        = "Reset modem hard. (battery out)";
        } // namespace cellular_passthrough
    }     // namespace window

    using namespace bsp::cellular::USB;

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

        bool set(PassthroughState pass_to_set, BootPinState dfu_to_set);

        void setWindowState(State state);

      private:
        void invalidate() noexcept;
    };
} // namespace gui
