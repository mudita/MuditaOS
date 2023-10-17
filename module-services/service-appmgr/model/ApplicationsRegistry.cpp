// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationsRegistry.hpp"

#include <apps-common/ApplicationLauncher.hpp>

namespace app::manager
{
    ApplicationsRegistry::ApplicationsRegistry(std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers)
    {
        std::vector<std::unique_ptr<app::ApplicationLauncher>> container = std::move(launchers);
        for (auto &&launcher : container) {
            applications.push_back(std::make_unique<ApplicationHandle>(std::move(launcher)));
        }
    }

    auto ApplicationsRegistry::getAll() const noexcept -> const ApplicationsContainer &
    {
        return applications;
    }

    auto ApplicationsRegistry::findByName(const ApplicationName &name) const -> ApplicationHandle *
    {
        auto it = std::find_if(
            applications.begin(), applications.end(), [&name](const auto &app) { return app->name() == name; });
        if (it == applications.end()) {
            return nullptr;
        }
        return it->get();
    }

    auto ApplicationsRegistry::findByAction(actions::ActionId action) const -> std::vector<ApplicationHandle *>
    {
        std::vector<ApplicationHandle *> actionHandlers;
        for (const auto &app : applications) {
            if (app->handles(action)) {
                actionHandlers.push_back(app.get());
            }
        }
        return actionHandlers;
    }
} // namespace app::manager
