// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationHandle.hpp"

#include <memory>
#include <vector>

namespace app::manager
{
    using ApplicationsContainer = std::vector<std::unique_ptr<ApplicationHandle>>;

    class ApplicationsRegistry
    {
      public:
        explicit ApplicationsRegistry(std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers);

        [[nodiscard]] auto getAll() const noexcept -> const ApplicationsContainer &;
        [[nodiscard]] auto findByName(const ApplicationName &name) const -> ApplicationHandle *;
        [[nodiscard]] auto findByAction(actions::ActionId action) const -> std::vector<ApplicationHandle *>;

      private:
        ApplicationsContainer applications;
    };
} // namespace app::manager
