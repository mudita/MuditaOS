/*
 * AntennaMAinWindow.hpp
 *
 *  Created on: 18 lut 2020
 *      Author: kuba
 */

#ifndef MODULE_APPS_APPLICATION_ANTENNA_WINDOWS_ANTENNAMAINWINDOW_HPP_
#define MODULE_APPS_APPLICATION_ANTENNA_WINDOWS_ANTENNAMAINWINDOW_HPP_

#include "Application.hpp"
#include "gui/widgets/Text.hpp"
#include "windows/AppWindow.hpp"

namespace gui
{

    class AntennaMainWindow : public AppWindow
    {
      protected:
        std::vector<gui::Label *> titles;

        std::vector<gui::Label *> buttons;
        gui::Text *operators;

        gui::Label *addLabel(const UTF8 &title, std::function<bool(Item &)> activatedCallback, bool visibleBorder = true);
        const std::string titlesText[4] = {"CSQ: ", "Status: ", "Band: ", "Operators: "};
        enum class labelDescripion
        {
            csq = 0,
            status = 1,
            band = 2,
            operators = 3
        };

      public:
        AntennaMainWindow(app::Application *app);
        virtual ~AntennaMainWindow();

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        void updateDebugInfo(std::vector<std::string> &data);
        void updateOperatorsScan(std::vector<std::string> &data);
    };

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_ANTENNA_WINDOWS_ANTENNAMAINWINDOW_HPP_ */
