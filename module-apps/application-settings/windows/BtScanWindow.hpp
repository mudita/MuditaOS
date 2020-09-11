#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <BoxLayout.hpp>
#include <memory>
#include <service-bluetooth/messages/BluetoothMessage.hpp>

namespace gui
{
    namespace name::window
    {
        inline const std::string name_btscan = "BT_SCAN";
    }

    class BtScanWindow : public AppWindow
    {
      protected:
        VBox *box;

      public:
        BtScanWindow(app::Application *app, std::vector<Devicei> devices = {});

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        void destroyInterface() override;

      private:
        void invalidate() noexcept;
        std::vector<Devicei> devices;
    };
} // namespace gui
