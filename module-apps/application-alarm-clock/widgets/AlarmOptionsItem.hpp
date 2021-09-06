// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmInternalListItem.hpp"

#include <Label.hpp>
#include <BoxLayout.hpp>
#include <widgets/TextSpinnerBox.hpp>

namespace gui
{
    class AlarmOptionsItem : public AlarmInternalListItem
    {
      public:
        explicit AlarmOptionsItem(const std::string &description, const std::vector<UTF8> &options = {});

      protected:
        gui::TextSpinnerBox *optionSpinner = nullptr;

      private:
        gui::VBox *vBox              = nullptr;
        gui::Label *descriptionLabel = nullptr;
    };

} /* namespace gui */
