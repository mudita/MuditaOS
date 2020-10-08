#pragma once

#include "AppWindow.hpp"
#include "Text.hpp"
#include "gui/widgets/Image.hpp"

namespace gui
{
    class LockedInfoWindow : public AppWindow
    {
        gui::Image *lockImage = nullptr;
        gui::Text *infoText   = nullptr;

        void setVisibleState();
        void invalidate() noexcept;

      public:
        explicit LockedInfoWindow(app::Application *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };
} /* namespace gui */
