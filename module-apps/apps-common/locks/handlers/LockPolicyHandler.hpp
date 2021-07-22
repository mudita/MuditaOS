// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <functional>
namespace app
{
    class ApplicationLauncher;
    class Application;
} // namespace app

namespace locks
{

    enum class AutoLockPolicy
    {
        DetermineByWindow,   /// Default value causing an application to be locked automatically if currently displayed
                             /// window does not prevent it.
        DetermineByAppState, /// Policy that checks if both window and current application state allows for autolock.
        PreventPermanently   /// Permanent state in which an application cannot be locked automatically. Once set it
                             /// cannot be unset.
    };

    class LockPolicyAccessInterface
    {
        AutoLockPolicy autoLockingPolicy = AutoLockPolicy::DetermineByWindow;

      protected:
        [[nodiscard]] AutoLockPolicy get() const noexcept;

      public:
        virtual ~LockPolicyAccessInterface() = default;
        void set(AutoLockPolicy policy) noexcept;
    };

    class LockPolicyHandlerInterface : public LockPolicyAccessInterface
    {
      protected:
        [[nodiscard]] virtual bool preventsAutoLockByWindow()      = 0;
        [[nodiscard]] virtual bool preventsAutoLockByState() const = 0;

      public:
        [[nodiscard]] bool preventsAutoLocking();
    };

    class LockPolicyHandler : public LockPolicyHandlerInterface
    {
        app::Application *owner = nullptr;
        std::function<bool()> preventsAutoLockByStateCallback;

        [[nodiscard]] bool preventsAutoLockByWindow() final;
        [[nodiscard]] bool preventsAutoLockByState() const final;

      public:
        explicit LockPolicyHandler(app::Application *owner,
                                   std::function<bool()> preventsAutoLockByStateCallback = nullptr) noexcept;

        void setPreventsAutoLockByStateCallback(std::function<bool()> _preventsAutoLockByStateCallback) noexcept;
    };

} // namespace locks
