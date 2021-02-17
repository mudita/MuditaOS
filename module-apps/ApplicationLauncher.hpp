// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"

namespace app
{
    using ApplicationManifest = app::manager::ApplicationManifest;

    enum class PreventAutoLocking : bool
    {
        False,
        True
    };
    enum class Closeable : bool
    {
        False,
        True
    };
    /// used in ApplicationManager to start applications
    class ApplicationLauncher
    {
      protected:
        /// name of the application to run
        std::string name;
        /// name of the application's owner
        std::string parent;
        /// Application's manifest
        ApplicationManifest manifest;
        /// defines whether application can be closed when it looses focus
        Closeable closeable = Closeable::False;
        /// defines whether application should be run without gaining focus, it will remian in the BACKGROUND state
        bool startBackground = false;
        /// flag defines whether this application can prevent auto-locking mechanism
        PreventAutoLocking preventAutoLocking = PreventAutoLocking::False;

      public:
        ApplicationLauncher(std::string name,
                            ApplicationManifest &&manifest,
                            Closeable isCloseable,
                            PreventAutoLocking preventAutoLocking = PreventAutoLocking::False)
            : name{std::move(name)}, manifest{std::move(manifest)}, closeable{isCloseable}, preventAutoLocking{
                                                                                                preventAutoLocking} {};
        virtual ~ApplicationLauncher() = default;

        [[nodiscard]] std::string getName() const noexcept
        {
            return name;
        }

        [[nodiscard]] const ApplicationManifest &getManifest() const noexcept
        {
            return manifest;
        }

        [[nodiscard]] bool isCloseable() const noexcept
        {
            return (closeable == Closeable::True);
        }

        [[nodiscard]] bool isPreventAutoLockingOn() const noexcept
        {
            return (preventAutoLocking == PreventAutoLocking::True);
        }

        virtual bool run(sys::Service *caller = nullptr)
        {
            return false;
        }

        virtual bool runBackground(sys::Service *caller = nullptr)
        {
            return false;
        }

        std::shared_ptr<Application> handle = nullptr;
    };

    /// application launcher boilerplate
    template <class T> class ApplicationLauncherT : public ApplicationLauncher
    {
      public:
        ApplicationLauncherT(std::string name,
                             ApplicationManifest &&manifest,
                             Closeable isCloseable              = Closeable::True,
                             PreventAutoLocking preventBlocking = PreventAutoLocking::True)
            : ApplicationLauncher(name, std::move(manifest), isCloseable, preventBlocking)
        {}

        bool run(sys::Service *caller) override
        {
            parent = (caller == nullptr ? "" : caller->GetName());
            handle = std::make_shared<T>(name, parent);
            return sys::SystemManager::RunService(handle, caller);
        }

        bool runBackground(sys::Service *caller) override
        {
            parent = (caller == nullptr ? "" : caller->GetName());
            handle = std::make_shared<T>(name, parent, true);
            return sys::SystemManager::RunService(handle, caller);
        }
    };

    /// creates application launcher per class provided
    template <class T>
    std::unique_ptr<ApplicationLauncherT<T>> CreateLauncher(
        std::string name,
        Closeable isCloseable                 = Closeable::True,
        PreventAutoLocking preventAutoLocking = PreventAutoLocking::False)
    {
        return std::unique_ptr<ApplicationLauncherT<T>>(
            new ApplicationLauncherT<T>(name, ManifestOf<T>(), isCloseable, preventAutoLocking));
    }
} // namespace app
