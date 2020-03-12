/*
 * @file ViewWindow.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 2 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_VIEWER_WINDOWS_VIEWWINDOW_HPP_
#define MODULE_APPS_APPLICATION_VIEWER_WINDOWS_VIEWWINDOW_HPP_

#include <map>
#include "AppWindow.hpp"
#include "widgets/Image.hpp"
#include "InputEvent.hpp"

namespace gui
{

    class ViewerState
    {
      public:
        std::string stateName;
        std::string screenName;
        bool deepRefresh = false;
        uint32_t id      = 0;
        std::map<int, int> buttons;

        ViewerState(std::string stateName, std::string screenName, bool deep, uint32_t id, std::map<int, int> buttons)
            : stateName{stateName}, screenName{screenName}, deepRefresh{deep}, id{id}, buttons{buttons} {};
    };

    /*
     *
     */
    class ViewWindow : public AppWindow
    {
        uint32_t currentState = 0;
        std::vector<gui::Image *> images;
        std::vector<ViewerState> states;

        int convertKeyCode(std::string);

        std::map<std::string, int> buttonKeyCodes = {
            {"KEY_UNDEFINED", static_cast<int>(KeyCode::KEY_UNDEFINED)},
            {"KEY_LEFT", static_cast<int>(KeyCode::KEY_LEFT)},
            {"KEY_UP", static_cast<int>(KeyCode::KEY_UP)},
            {"KEY_RIGHT", static_cast<int>(KeyCode::KEY_RIGHT)},
            {"KEY_DOWN", static_cast<int>(KeyCode::KEY_DOWN)},
            {"KEY_LF", static_cast<int>(KeyCode::KEY_LF)},
            {"KEY_RF", static_cast<int>(KeyCode::KEY_RF)},
            {"KEY_ENTER", static_cast<int>(KeyCode::KEY_ENTER)},
            {"KEY_0", static_cast<int>(KeyCode::KEY_0)},
            {"KEY_1", static_cast<int>(KeyCode::KEY_1)},
            {"KEY_2", static_cast<int>(KeyCode::KEY_2)},
            {"KEY_3", static_cast<int>(KeyCode::KEY_3)},
            {"KEY_4", static_cast<int>(KeyCode::KEY_4)},
            {"KEY_5", static_cast<int>(KeyCode::KEY_5)},
            {"KEY_6", static_cast<int>(KeyCode::KEY_6)},
            {"KEY_7", static_cast<int>(KeyCode::KEY_7)},
            {"KEY_8", static_cast<int>(KeyCode::KEY_8)},
            {"KEY_9", static_cast<int>(KeyCode::KEY_9)},
            {"KEY_AST", static_cast<int>(KeyCode::KEY_AST)},
            {"KEY_PND", static_cast<int>(KeyCode::KEY_PND)},
            {"KEY_VOLUP", static_cast<int>(KeyCode::KEY_VOLUP)},
            {"KEY_VOLDN", static_cast<int>(KeyCode::KEY_VOLDN)},
            {"KEY_TORCH", static_cast<int>(KeyCode::KEY_TORCH)},
            {"SWITCH_UP", static_cast<int>(KeyCode::SWITCH_UP)},
            {"SWITCH_MID", static_cast<int>(KeyCode::SWITCH_MID)},
            {"SWITCH_DN", static_cast<int>(KeyCode::SWITCH_DN)},
        };

      public:
        ViewWindow(app::Application *app);
        virtual ~ViewWindow();

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_VIEWER_WINDOWS_VIEWWINDOW_HPP_ */
