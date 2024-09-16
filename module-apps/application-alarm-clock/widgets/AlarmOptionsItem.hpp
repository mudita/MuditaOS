// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "AlarmInternalListItem.hpp"

#include <Label.hpp>
#include <BoxLayout.hpp>
#include <widgets/TextSpinnerBoxWithLabel.hpp>

namespace gui
{
    class AlarmOptionsItem : public AlarmInternalListItem
    {
      public:
        explicit AlarmOptionsItem(const std::string &description, const std::vector<UTF8> &options = {});

      protected:
        gui::TextSpinnerBoxWithLabel *optionSpinner = nullptr;
    };

} /* namespace gui */
