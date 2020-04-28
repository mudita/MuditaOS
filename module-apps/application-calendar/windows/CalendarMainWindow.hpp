#pragma once

#include "windows/AppWindow.hpp"
#include "Application.hpp"

namespace app
{

    /*
     *
     */
    class CalendarMainWindow : public gui::AppWindow
    {
      public:
        CalendarMainWindow(Application *app, std::string name);
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace app */
