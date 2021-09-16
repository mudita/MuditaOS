// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SpecialCharactersTableStyle.hpp"
#include "SpecialInputMainWindow.hpp"
#include <apps-common/ApplicationCommon.hpp>
#include <ListItem.hpp>
#include <Text.hpp>
#include <TextFixedSize.hpp>
#include <ImageBox.hpp>
#include <GridLayout.hpp>

namespace gui
{

    struct Carrier
    {
        gui::Item *item = nullptr;
        std::string val;
    };

    auto generateNewLineSign() -> Carrier;
    auto generateCharSign(uint32_t val) -> Carrier;

    class SpecialInputTableWidget : public ListItem
    {
      protected:
        void decorateActionActivated(Item *it, const std::string &str);
        GridLayout *box               = nullptr;
        app::ApplicationCommon *application = nullptr;

      public:
        SpecialInputTableWidget(app::ApplicationCommon *app, std::list<Carrier> &&carier);
    };

} /* namespace gui */
