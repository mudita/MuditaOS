// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <ListItem.hpp>

namespace gui
{

    class MultipleNumbersWidget : public ListItem
    {
      private:
        VBox *vBox = nullptr;

      public:
        MultipleNumbersWidget(const std::string &description, const std::string &number);
    };
} /* namespace gui */
