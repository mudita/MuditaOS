// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ActionsRegistry.hpp"

#include <log.hpp>
#include <magic_enum.hpp>

#include <cassert>
#include <stdexcept>

namespace app::manager
{
    namespace
    {
        constexpr auto ExpirationTimeout = std::chrono::seconds{30};
    } // namespace

    ActionEntry::ActionEntry(actions::ActionId action, actions::ActionParamsPtr &&params, Timestamp creationTime)
        : actionId{action}, params{std::move(params)}, createdAt{creationTime}
    {}

    void ActionEntry::setTargetApplication(std::string targetApplication) noexcept
    {
        target = std::move(targetApplication);
    }

    auto ActionEntry::isExpired() const noexcept -> bool
    {
        const auto diff = Clock::now() - createdAt;
        return diff > ExpirationTimeout;
    }

    bool operator==(const ActionEntry &lhs, const ActionEntry &rhs) noexcept
    {
        return lhs.actionId == rhs.actionId && lhs.createdAt == rhs.createdAt && lhs.params == rhs.params;
    }

    bool operator!=(const ActionEntry &lhs, const ActionEntry &rhs) noexcept
    {
        return !operator==(lhs, rhs);
    }

    ActionsRegistry::ActionsRegistry(OnNextActionReadyCallback &&callback) : nextActionReady{std::move(callback)}
    {
        if (!nextActionReady) {
            throw std::invalid_argument{"No OnNextActionReadyCallback passed."};
        }
    }

    void ActionsRegistry::enqueue(ActionEntry &&action)
    {
        LOG_INFO("Enqueue action %s", magic_enum::enum_name(action.actionId).data());

        addAction(std::move(action));
        if (!isCurrentlyProcessing()) {
            notifyAboutNextAction();
        }
    }

    void ActionsRegistry::addAction(ActionEntry &&action)
    {
        actions.push_back(std::move(action));
    }

    auto ActionsRegistry::isCurrentlyProcessing() const noexcept -> bool
    {
        return hasPendingAction();
    }

    void ActionsRegistry::notifyAboutNextAction()
    {
        for (auto it = actions.begin(); it != actions.end();) {
            auto &action = *it;
            switch (const auto status = nextActionReady(action); status) {
            case ActionProcessStatus::Accepted: {
                LOG_INFO(
                    "Pending action %s to %s", magic_enum::enum_name(action.actionId).data(), action.target.c_str());
                actionInProgress = &action;
                return;
            }
            case ActionProcessStatus::Dropped: {
                // Drop the action, so it'll not be processed again.
                it = actions.erase(it);
                break;
            }
            case ActionProcessStatus::Skipped:
                // Skip the action and check the next one.
                ++it;
                break;
            }
        }
    }

    void ActionsRegistry::finished()
    {
        assert(isCurrentlyProcessing());

        LOG_INFO("Finished action %s on %s",
                 magic_enum::enum_name(actionInProgress->actionId).data(),
                 actionInProgress->target.c_str());
        if (const auto it = std::find(actions.begin(), actions.end(), *actionInProgress); it != actions.end()) {
            actions.erase(it);
        }

        actionInProgress = nullptr;
        onFinished();
    }

    void ActionsRegistry::onFinished()
    {
        removeExpiredActions();
        if (!actions.empty()) {
            notifyAboutNextAction();
        }
    }

    void ActionsRegistry::removeExpiredActions()
    {
        actions.erase(
            std::remove_if(actions.begin(), actions.end(), [](const auto &action) { return action.isExpired(); }),
            actions.end());
    }

    auto ActionsRegistry::hasPendingAction() const noexcept -> bool
    {
        return actionInProgress != nullptr;
    }

    auto ActionsRegistry::getPendingAction() noexcept -> ActionEntry *
    {
        return actionInProgress;
    }
} // namespace app::manager
