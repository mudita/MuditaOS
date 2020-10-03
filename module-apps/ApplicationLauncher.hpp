#pragma once

#include "Application.hpp"

namespace app
{

    /// used in ApplicationManager to start applications
    class ApplicationLauncher
    {
      protected:
        /// name of the application to run
        std::string name;
        /// name of the application's owner
        std::string parent;
        /// defines whether application can be closed when it looses focus
        bool closeable = true;
        /// defines whether application should be run without gaining focus, it will remian in the BACKGROUND state
        bool startBackground = false;
        /// flag defines whether this application can prevent power manager from changing
        bool preventBlocking = false;

      public:
        ApplicationLauncher(std::string name, bool isCloseable, bool preventBlocking = false)
            : name{name}, closeable{isCloseable}, preventBlocking{preventBlocking} {};
        virtual ~ApplicationLauncher() = default;

        [[nodiscard]] std::string getName() const noexcept
        {
            return name;
        }

        [[nodiscard]] bool isCloseable() const noexcept
        {
            return closeable;
        }

        [[nodiscard]] bool isBlocking() const noexcept
        {
            return preventBlocking;
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
        ApplicationLauncherT(std::string name, bool isCloseable = true) : ApplicationLauncher(name, isCloseable)
        {}

        bool run(sys::Service *caller) override
        {
            parent = (caller == nullptr ? "" : caller->GetName());
            handle = std::make_shared<T>(name, parent);
            return sys::SystemManager::CreateService(handle, caller);
        }

        bool runBackground(sys::Service *caller) override
        {
            parent = (caller == nullptr ? "" : caller->GetName());
            handle = std::make_shared<T>(name, parent, true);
            return sys::SystemManager::CreateService(handle, caller);
        }
    };

    /// creates application launcher per class provided
    template <class T>
    std::unique_ptr<ApplicationLauncherT<T>> CreateLauncher(std::string name, bool isCloseable = true)
    {
        return std::unique_ptr<ApplicationLauncherT<T>>(new ApplicationLauncherT<T>(name, isCloseable));
    }
} // namespace app
