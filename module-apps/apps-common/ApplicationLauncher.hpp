// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace app
{
    using ApplicationManifest = app::manager::ApplicationManifest;

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

        void setAutoLockPolicy() noexcept
        {
            if (handle != nullptr) {
                handle->getLockPolicyHandler().set(manifest.getAutoLockPolicy());
            }
        }

      public:
        ApplicationLauncher(std::string name, ApplicationManifest &&manifest, Closeable isCloseable)
            : name{std::move(name)}, manifest{std::move(manifest)}, closeable{isCloseable} {};
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

        [[nodiscard]] bool preventsAutoLocking() const noexcept
        {
            return (handle == nullptr || handle->getLockPolicyHandler().preventsAutoLocking());
        }

        virtual bool run(StatusIndicators statusIndicators, sys::Service *caller = nullptr)
        {
            return false;
        }

        virtual bool runBackground(StatusIndicators statusIndicators, sys::Service *caller = nullptr)
        {
            return false;
        }

        std::shared_ptr<ApplicationCommon> handle = nullptr;
    };

    /// application launcher boilerplate
    template <class T> class ApplicationLauncherT : public ApplicationLauncher
    {
      public:
        ApplicationLauncherT(std::string name, ApplicationManifest &&manifest, Closeable isCloseable = Closeable::True)
            : ApplicationLauncher(std::move(name), std::move(manifest), isCloseable)
        {}

        bool run(StatusIndicators statusIndicators, sys::Service *caller) override
        {
            parent = (caller == nullptr ? "" : caller->GetName());
            handle = std::make_shared<T>(name, parent, statusIndicators);
            setAutoLockPolicy();
            return sys::SystemManagerCommon::RunApplication(handle, caller);
        }

        bool runBackground(StatusIndicators statusIndicators, sys::Service *caller) override
        {
            parent = (caller == nullptr ? "" : caller->GetName());
            handle = std::make_shared<T>(name, parent, statusIndicators, true);
            setAutoLockPolicy();
            return sys::SystemManagerCommon::RunApplication(handle, caller);
        }
    };

    /// creates application launcher per class provided
    template <class T>
    std::unique_ptr<ApplicationLauncherT<T>> CreateLauncher(std::string name, Closeable isCloseable = Closeable::True)
    {
        return std::unique_ptr<ApplicationLauncherT<T>>(
            new ApplicationLauncherT<T>(name, ManifestOf<T>(), isCloseable));
    }
} // namespace app
