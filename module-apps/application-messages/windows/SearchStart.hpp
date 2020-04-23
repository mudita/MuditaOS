#pragma once

#include "AppWindow.hpp"

namespace gui
{
    class SMSSearch : public AppWindow
    {
        gui::Item *body = nullptr;

      public:
        SMSSearch(app::Application *app);
        bool onInput(const InputEvent &inputEvent) override;
    };

} // namespace gui
