// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WindowsFactory.hpp"
#include <AppWindow.hpp>

namespace app
{

    void WindowsFactory::attach(const std::string &name, builder builder)
    {
        builders[name] = builder;
    }

    auto WindowsFactory::isRegistered(const std::string &name) const -> bool
    {
        return builders.find(name) != std::end(builders);
    }

    auto WindowsFactory::build(ApplicationCommon *app, const std::string &name) -> handle
    {
        return builders[name](app, name);
    }
} // namespace app
