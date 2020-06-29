#pragma once

#include "EnterNumberWindow.hpp"

namespace gui
{

    class EmergencyCallWindow : public EnterNumberWindow
    {
      public:
        EmergencyCallWindow(app::Application *app);

        bool onInput(const InputEvent &inputEvent) override;
        bool handleSwitchData(SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
