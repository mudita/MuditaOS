// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/model/ApplicationHandle.hpp>

namespace app::manager
{
    ApplicationHandle::ApplicationHandle(std::unique_ptr<app::ApplicationLauncher> &&_launcher)
        : launcher{std::move(_launcher)}
    {}

    auto ApplicationHandle::manifest() const -> const ApplicationManifest &
    {
        return launcher ? launcher->getManifest() : InvalidManifest;
    }

    auto ApplicationHandle::name() const -> ApplicationName
    {
        return launcher ? launcher->getName() : InvalidAppName.data();
    }

    auto ApplicationHandle::state() const noexcept -> State
    {
        return launcher && launcher->handle ? launcher->handle->getState() : State::NONE;
    }

    void ApplicationHandle::setState(State state) noexcept
    {
        if (launcher && launcher->handle) {
            launcher->handle->setState(state);
        }
    }

    auto ApplicationHandle::preventsBlocking() const noexcept -> bool
    {
        return launcher ? launcher->isBlocking() : false;
    }

    auto ApplicationHandle::closeable() const noexcept -> bool
    {
        return launcher ? launcher->isCloseable() : false;
    }

    auto ApplicationHandle::started() const noexcept -> bool
    {
        const auto appState = state();
        return appState == State::ACTIVE_FORGROUND || appState == State::ACTIVE_BACKGROUND ||
               appState == State::ACTIVATING;
    }

    auto ApplicationHandle::handles(actions::ActionId action) const noexcept -> bool
    {
        return manifest().contains(action);
    }

    void ApplicationHandle::run(sys::Service *caller)
    {
        if (launcher) {
            launcher->run(caller);
        }
    }

    void ApplicationHandle::runInBackground(sys::Service *caller)
    {
        if (launcher) {
            launcher->runBackground(caller);
        }
    }

    void ApplicationHandle::close() noexcept
    {
        if (launcher) {
            launcher->handle = nullptr;
        }
    }
} // namespace app::manager
