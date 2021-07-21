// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <Image.hpp>
#include <ListItem.hpp>

namespace gui
{
    class SettingsSoundItem : public ListItem
    {

      public:
        SettingsSoundItem(const std::string &soundName, bool selected);

      private:
        HBox *mMainBox     = nullptr;
        Image *mIsSelected = nullptr;
    };

} /* namespace gui */
