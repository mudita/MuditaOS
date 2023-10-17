// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <functional>
#include <map>
#include <string>

namespace app
{
    class ApplicationCommon;
};

namespace gui
{
    class AppWindow;
}

namespace app
{
    class WindowsFactory
    {
      public:
        using handle  = std::unique_ptr<gui::AppWindow>;
        using builder = std::function<handle(ApplicationCommon *, std::string)>;

      private:
        std::map<std::string, builder> builders;

      public:
        WindowsFactory()                       = default;
        WindowsFactory(const WindowsFactory &) = delete;
        WindowsFactory(WindowsFactory &&)      = delete;
        WindowsFactory &operator=(const WindowsFactory &) = delete;
        WindowsFactory &operator=(WindowsFactory &&) = delete;

        void attach(const std::string &name, builder builder);
        [[nodiscard]] auto isRegistered(const std::string &name) const -> bool;
        auto build(ApplicationCommon *app, const std::string &name) -> handle;
    };
} // namespace app
