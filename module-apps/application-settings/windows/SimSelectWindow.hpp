// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <OptionWindow.hpp>
namespace app
{
    class Application;
    class SimSetter;
} // namespace app

std::list<gui::Option> simSelectWindow(app::Application *app, app::SimSetter *setter);