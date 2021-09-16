// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Option.hpp"
#include "OptionsModel.hpp"

#include <ListView.hpp>

namespace gui
{
    class OptionsList
    {
      protected:
        explicit OptionsList(std::shared_ptr<OptionsModel>(app), std::list<Option> = {});

        ListView *optionsList                      = nullptr;
        std::shared_ptr<OptionsModel> optionsModel = nullptr;
        std::list<Option> options;

        void createOptions();
        void recreateOptions();
        void clearOptions();
        void addOptions(std::list<Option> &&optionList);
        void changeOptions(std::list<Option> &&optionList);
        void refreshOptions(std::list<Option> &&optionList);
        void refreshOptions(std::list<Option> &&optionList, unsigned int pageIndex);
    };
} // namespace gui
