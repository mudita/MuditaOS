// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "AppWindow.hpp"

#include <TextFixedSize.hpp>
#include <widgets/TextSpinnerBox.hpp>
#include <SystemManager/CpuSentinel.hpp>

namespace gui
{
    class CPUModeTestWindow : public AppWindow
    {
      private:
        static inline auto name = "CPU Mode Tester";

        VBox *body                           = nullptr;
        TextFixedSize *currentFreqValue      = nullptr;
        TextSpinnerBox *currentFreqSpinner   = nullptr;
        TextSpinnerBox *permanentFreqSpinner = nullptr;

        std::shared_ptr<sys::CpuSentinel> cpuModeTester;

        void createCurrentFreqBox();
        void createPermanentFreqSettingBox();
        void createNewFreqBox();

      public:
        explicit CPUModeTestWindow(app::ApplicationCommon *app);
        void onClose(CloseReason reason) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };
} /* namespace gui */
