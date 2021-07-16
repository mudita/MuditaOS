// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/Actions.hpp>

#include <chrono>
#include <deque>
#include <functional>

namespace app::manager
{
    struct ActionEntry
    {
      public:
        using Clock     = std::chrono::steady_clock;
        using Timestamp = std::chrono::time_point<Clock>;

        explicit ActionEntry(actions::ActionId action,
                             actions::ActionParamsPtr &&params = {},
                             Timestamp creationTime            = Clock::now());

        void setTargetApplication(std::string targetApplication) noexcept;
        [[nodiscard]] auto isExpired() const noexcept -> bool;

        actions::Action actionId;
        actions::ActionParamsPtr params;
        std::string target;

      private:
        friend bool operator==(const ActionEntry &lhs, const ActionEntry &rhs) noexcept;
        friend bool operator!=(const ActionEntry &lhs, const ActionEntry &rhs) noexcept;

        Timestamp createdAt = Clock::now();
    };

    bool operator==(const ActionEntry &lhs, const ActionEntry &rhs) noexcept;
    bool operator!=(const ActionEntry &lhs, const ActionEntry &rhs) noexcept;

    enum class ActionProcessStatus
    {
        Accepted,
        Skipped,
        Dropped
    };

    class ActionsRegistry
    {
      public:
        using OnNextActionReadyCallback = std::function<ActionProcessStatus(ActionEntry &action)>;

        explicit ActionsRegistry(OnNextActionReadyCallback &&callback);

        void enqueue(ActionEntry &&action);
        void finished();

        [[nodiscard]] auto hasPendingAction() const noexcept -> bool;
        [[nodiscard]] auto getPendingAction() noexcept -> ActionEntry *;

      private:
        [[nodiscard]] auto isCurrentlyProcessing() const noexcept -> bool;
        void addAction(ActionEntry &&action);
        void notifyAboutNextAction();
        void removeExpiredActions();
        void onFinished();

        std::deque<ActionEntry> actions;
        OnNextActionReadyCallback nextActionReady;
        ActionEntry *actionInProgress = nullptr;
    };
} // namespace app::manager
