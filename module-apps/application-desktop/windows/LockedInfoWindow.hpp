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
        // method hides or show widgets and sets bars according to provided state
        void setVisibleState();
        void invalidate() noexcept;
      public:
        LockedInfoWindow(app::Application *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
