// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace app
{
    class Application;
    class PinLockSetter;
} // namespace app

#include <OptionWindow.hpp>

std::list<gui::Option> settingsChangeWindow(app::Application *app, app::PinLockSetter *setter, unsigned int lockHash);
