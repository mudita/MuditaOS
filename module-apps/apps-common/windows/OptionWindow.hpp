// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationCommon.hpp"
#include "AppWindow.hpp"
#include "OptionsList.hpp"
#include "OptionWindowName.hpp"

namespace gui
{
    ///  @brief Options window generating various options based on provided Option list.
    ///
    ///  Options GUI window with ListView populated accordingly to provided options in window constructor.
    ///

    class OptionWindow : public AppWindow, protected OptionsList<ListView>
    {
      private:
        const std::string windowTitle;

      public:
        OptionWindow(app::ApplicationCommon *app, const std::string &name, const std::string &windowTitle = "");
        OptionWindow(app::ApplicationCommon *app,
                     const std::string &name,
                     std::list<Option> options,
                     const std::string &windowTitle = "");

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        void rebuild() override;
        void buildInterface() override;
    };
}; // namespace gui
