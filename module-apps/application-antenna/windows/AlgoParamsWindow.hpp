// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-antenna/ApplicationAntenna.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/windows/AppWindow.hpp>
#include <module-gui/gui/widgets/Text.hpp>

namespace gui
{
    namespace name
    {
        namespace window
        {
            inline constexpr auto algo_window = "AlgoParamsWindow";
        }
    } // namespace name
    class AlgoParamsWindow : public AppWindow
    {
      protected:
        std::list<std::string> lowBandParams;
        std::list<std::string> highBandParams;

        gui::VBox *lowBandBox        = nullptr;
        gui::VBox *highBandBox       = nullptr;
        app::ApplicationAntenna *app = nullptr;
        gui::Label *selectedAntenna  = nullptr;
        gui::Label *info             = nullptr;
        gui::Label *lowBandLabel     = nullptr;
        gui::Label *highBandLabel    = nullptr;

        std::string selectedAntennaPrefix = "Currently selected antenna: ";
        std::string lowBandString         = "Low Band";
        std::string highBandString        = "High Band";

        gui::Label *addLabel(gui::Item *parent, const UTF8 &text);
        void updateAntennaLabel(bsp::cellular::antenna antenna);

      public:
        AlgoParamsWindow(app::ApplicationCommon *app);

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        void handleNewParams(std::string data, bool refresh);
        void handleAntennaChanged(bsp::cellular::antenna antenna, bool refresh);
    };

} /* namespace gui */
