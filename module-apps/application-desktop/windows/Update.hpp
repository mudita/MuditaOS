#pragma once

#include <vector>
#include <module-gui/gui/widgets/Progress.hpp>
#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/BottomBar.hpp"

namespace gui
{
    class UpdateWindow : public AppWindow
    {

        enum class State
        {
            UpdateNow,
            Return,
        };

        gui::Label *titleLabel = nullptr;
        gui::Label *infoLabel  = nullptr;

        std::vector<gui::Label *> selectionLabels;
        gui::Image *powerImage     = nullptr;
        gui::Image *powerDownImage = nullptr;
        gui::Progress *progressBar  = nullptr;
        State state                = State::Return;

      public:
        UpdateWindow(app::Application *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        bool handleSwitchData(SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

        fs::path updateFile;
    };

} /* namespace gui */
