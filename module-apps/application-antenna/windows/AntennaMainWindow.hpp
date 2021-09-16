// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/windows/AppWindow.hpp>
#include <module-gui/gui/widgets/Text.hpp>
#include <service-antenna/ServiceAntenna.hpp>

namespace gui
{

    class AntennaMainWindow : public AppWindow
    {
      protected:
        std::vector<gui::Label *> titles;

        std::vector<gui::Label *> buttons;
        gui::Text *operators = nullptr;

        gui::Label *addLabel(const UTF8 &title,
                             std::function<bool(Item &)> activatedCallback,
                             bool visibleBorder = true);
        const std::string titlesText[4] = {"CSQ: ", "Status: ", "Band: ", "Operators: "};
        enum class labelDescripion
        {
            csq       = 0,
            status    = 1,
            band      = 2,
            operators = 3
        };

        enum buttonDescriotion
        {
            AntennaA = 0,
            AntennaB,
            StartScan,
            ScanMode,
            LockAntennaManager,
            AlgoParams
        };

      public:
        AntennaMainWindow(app::ApplicationCommon *app);

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        void updateDebugInfo(std::vector<std::string> &data);
        void updateOperatorsScan(std::vector<std::string> &data);
        void updateAntennaButtons(bsp::cellular::antenna antenna);
        void updateLockedButton(antenna::lockState antennaState);
    };

} /* namespace gui */

