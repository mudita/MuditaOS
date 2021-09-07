// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "EntryPath.hpp"
#include "Service/ServiceProxy.hpp"
#include <functional>

namespace settings
{
    class SettingsProxy : private service::ServiceProxy
    {
      public:
        SettingsProxy() = default;
        ~SettingsProxy();
        explicit SettingsProxy(const service::ServiceProxy &interface);
        void init(std::function<void(EntryPath, std::string)> onChangeHandler);
        void deinit();
        [[nodiscard]] bool isValid() const noexcept;

        void onChange(EntryPath path, std::string value);
        void registerValueChange(EntryPath path);
        void unregisterValueChange(EntryPath path);
        void setValue(const EntryPath &path, const std::string &value);
        [[nodiscard]] std::string ownerName();

      private:
        std::function<void(EntryPath, std::string)> onChangeHandler;
    };
} // namespace settings
