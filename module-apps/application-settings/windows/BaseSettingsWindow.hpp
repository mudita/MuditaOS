#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <memory>
#include <BoxLayout.hpp>

namespace gui
{

    class SettingsWindow : public AppWindow
    {
      public:
        SettingsWindow(app::Application *app, std::string name);

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      protected:
        virtual void invalidate() noexcept {};
    };
} // namespace gui
