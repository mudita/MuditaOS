#pragma once

#include <vector>
#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/BottomBar.hpp"

namespace gui
{
    class PowerOffWindow : public AppWindow
    {

        enum class State
        {
            PowerDown,
            Return,
        };

        gui::Label *titleLabel = nullptr;
        gui::Label *infoLabel  = nullptr;

        std::vector<gui::Label *> selectionLabels;
        gui::Label *eventMgrLabel  = nullptr;
        gui::Image *powerImage     = nullptr;
        gui::Image *powerDownImage = nullptr;
        State state                = State::Return;

      public:
        PowerOffWindow(app::Application *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
