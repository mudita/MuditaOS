// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/data/SettingsItemData.hpp>

#include <gui/widgets/ListItem.hpp>

namespace gui
{
    using ApnListItem = ListItemWithCallbacks<packet_data::APN::Config>;
} /* namespace gui */
