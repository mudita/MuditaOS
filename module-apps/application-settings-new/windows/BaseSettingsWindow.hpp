#pragma once

#include <AppWindow.hpp>
#include <Application.hpp>
#include <module-apps/windows/OptionWindow.hpp>

namespace gui
{

    class BaseSettingsWindow : public AppWindow
    {
      public:
        BaseSettingsWindow(app::Application *app, std::string name);

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      protected:
        virtual void invalidate() noexcept {};
    };
} // namespace gui
