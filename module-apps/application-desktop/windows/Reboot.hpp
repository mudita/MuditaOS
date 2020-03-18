#pragma once

#include <vector>
#include "AppWindow.hpp"
#include <Text.hpp>

namespace gui
{
    class RebootWindow : public AppWindow
    {

        Text *text = nullptr;

      public:
        RebootWindow(app::Application *app);
        ~RebootWindow() override = default;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
