#pragma once

#include "AppWindow.hpp"

namespace gui
{

    namespace name::window
    {
        inline std::string thread_sms_search = "SMSSearch";
    }

    class SMSSearch : public AppWindow
    {
        gui::Item *body = nullptr;

      public:
        SMSSearch(app::Application *app);
        bool onInput(const InputEvent &inputEvent) override;
    };

} // namespace gui
