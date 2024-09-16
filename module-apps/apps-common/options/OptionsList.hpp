// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Option.hpp"
#include "OptionsModel.hpp"

#include <ListView.hpp>
#include <ListViewWithArrows.hpp>

namespace gui
{
    template <class ListType>
    class OptionsList
    {
      protected:
        explicit OptionsList(std::shared_ptr<OptionsModel>(app), std::list<Option> = {});

        ListType *optionsList                      = nullptr;
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
