/*
 * AntennaMAinWindow.hpp
 *
 *  Created on: 18 lut 2020
 *      Author: kuba
 */

#ifndef MODULE_APPS_APPLICATION_ANTENNA_WINDOWS_ANTENNAMAINWINDOW_HPP_
#define MODULE_APPS_APPLICATION_ANTENNA_WINDOWS_ANTENNAMAINWINDOW_HPP_

#include "Application.hpp"
#include "windows/AppWindow.hpp"

namespace gui
{

    class AntennaMainWindow : public AppWindow
    {
      protected:
      public:
        AntennaMainWindow(app::Application *app);
        virtual ~AntennaMainWindow();

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_ANTENNA_WINDOWS_ANTENNAMAINWINDOW_HPP_ */
