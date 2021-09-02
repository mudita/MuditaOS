// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ListItem.hpp>
#include <Text.hpp>

namespace gui
{
    class TechnicalInformationItem : public ListItem
    {
      public:
        TechnicalInformationItem(const std::string &title, const std::string &value);

      private:
        VBox *body  = nullptr;
        Text *title = nullptr;
        Text *value = nullptr;
    };

} /* namespace gui */
