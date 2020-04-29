/*
 * TestMessageWindow.hpp
 *
 *  Created on: 3 gru 2019
 *      Author: kuba
 */

#ifndef MODULE_APPS_APPLICATION_SETTINGS_WINDOWS_TESTMESSAGEWINDOW_HPP_
#define MODULE_APPS_APPLICATION_SETTINGS_WINDOWS_TESTMESSAGEWINDOW_HPP_

#include <string>
#include <functional>

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Text.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

namespace gui
{

    /*
     *
     */
    class TestMessageWindow : public AppWindow
    {
      protected:
        gui::Label *sendLabel;
        gui::Label *sendButton;

        gui::Label *receivedLabel;

        gui::Label *SendNumberLabel;
        gui::Label *SendMessageLabel;
        gui::Text *sendNumber;
        gui::Text *sendMessage;

        gui::Label *RecNumberLabel;
        gui::Label *RecMessageLabel;
        gui::Label *receiveNumber;
        gui::Text *receiveMessage;

      public:
        TestMessageWindow(app::Application *app);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        void cellularMessageCallback(UTF8 &number, UTF8 &message);
    };
} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_SETTINGS_WINDOWS_TESTMESSAGEWINDOW_HPP_ */
