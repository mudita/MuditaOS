// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include "AppWindow.hpp"
#include "ListView.hpp"
#include <functional>
#include "Option.hpp"
#include "OptionsModel.hpp"

namespace gui
{
    ///  @brief Options window generating various options based on provided Option list.
    ///
    ///  Options GUI window with ListView populated accordingly to provided options in window constructor.
    ///

    class OptionWindow : public AppWindow
    {
      protected:
        std::shared_ptr<OptionsModel> optionsModel = nullptr;
        ListView *optionsList                      = nullptr;
        std::list<Option> options;

        void createOptions();
        void recreateOptions();
        void clearOptions();
        void addOptions(std::list<Option> &&optionList);
        void changeOptions(std::list<Option> &&optionList);
        void refreshOptions(std::list<Option> &&optionList);
        void refreshOptions(std::list<Option> &&optionList, unsigned int pageIndex);

      public:
        OptionWindow(app::Application *app, const std::string &name);
        OptionWindow(app::Application *app, const std::string &name, std::list<Option> options);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        void rebuild() override;
        void buildInterface() override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;
    };
}; // namespace gui
