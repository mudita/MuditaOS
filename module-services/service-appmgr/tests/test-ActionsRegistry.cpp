// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <service-appmgr/model/ActionsRegistry.hpp>

using namespace app::manager;

TEST_CASE("Given actions registry when initialise it incorrectly then verify")
{
    REQUIRE_THROWS(ActionsRegistry{nullptr});
}

TEST_CASE("Given empty actions registry when verify its state then no pending actions")
{
    auto nextActionCallback = [](ActionEntry & /*entry*/) { return ActionProcessStatus::Accepted; };
    ActionsRegistry registry{std::move(nextActionCallback)};

    REQUIRE(!registry.hasPendingAction());
}

TEST_CASE("Given actions registry when enqueue then check pending action")
{
    auto nextActionCallback = [](ActionEntry & /*entry*/) { return ActionProcessStatus::Accepted; };
    ActionsRegistry registry{std::move(nextActionCallback)};

    registry.enqueue(ActionEntry{actions::Launch});
    REQUIRE(registry.hasPendingAction());
    REQUIRE(registry.getPendingAction()->actionId == actions::Launch);
}

TEST_CASE("Given actions registry when enqueue then check if action was processed")
{
    bool handled            = false;
    auto nextActionCallback = [&handled](ActionEntry & /*entry*/) {
        handled = true;
        return ActionProcessStatus::Accepted;
    };
    ActionsRegistry registry{std::move(nextActionCallback)};

    registry.enqueue(ActionEntry{actions::Launch});
    REQUIRE(handled);
}

TEST_CASE("Given actions registry when finished queued action then no pending actions")
{
    auto nextActionCallback = [](ActionEntry & /*entry*/) { return ActionProcessStatus::Accepted; };
    ActionsRegistry registry{std::move(nextActionCallback)};

    registry.enqueue(ActionEntry{actions::Launch});
    registry.finished();
    REQUIRE(!registry.hasPendingAction());
}

TEST_CASE("Given actions registry when enqueue multiple actions sequentially then count handled actions")
{
    int counter             = 0;
    auto nextActionCallback = [&counter](ActionEntry & /*entry*/) {
        ++counter;
        return ActionProcessStatus::Accepted;
    };
    ActionsRegistry registry{std::move(nextActionCallback)};

    registry.enqueue(ActionEntry{actions::Launch});
    registry.finished();

    registry.enqueue(ActionEntry{actions::Launch});
    registry.finished();

    registry.enqueue(ActionEntry{actions::Launch});
    registry.finished();

    REQUIRE(counter == 3);
}

TEST_CASE("Given actions registry when enqueue multiple actions at once then count handled actions")
{
    int counter             = 0;
    auto nextActionCallback = [&counter](ActionEntry & /*entry*/) {
        ++counter;
        return ActionProcessStatus::Accepted;
    };
    ActionsRegistry registry{std::move(nextActionCallback)};

    registry.enqueue(ActionEntry{actions::Launch});
    REQUIRE(counter == 1);
    REQUIRE(registry.hasPendingAction());

    registry.enqueue(ActionEntry{actions::Launch});
    registry.enqueue(ActionEntry{actions::Launch});

    registry.finished(); // Finished processing the 1st action
    REQUIRE(counter == 2);
    REQUIRE(registry.hasPendingAction());

    registry.finished(); // Finished processing the 2nd action
    REQUIRE(counter == 3);
    REQUIRE(registry.hasPendingAction());

    registry.finished(); // Finished processing the 3rd action
    REQUIRE(!registry.hasPendingAction());
}

TEST_CASE("Given actions registry when enqueue multiple actions at once then wait for them to expire.")
{
    auto nextActionCallback = [](ActionEntry & /*entry*/) { return ActionProcessStatus::Accepted; };
    ActionsRegistry registry{std::move(nextActionCallback)};

    registry.enqueue(ActionEntry{actions::Launch});
    REQUIRE(registry.hasPendingAction());

    registry.enqueue(ActionEntry{actions::Launch, {}, std::chrono::steady_clock::now() - std::chrono::seconds{29}});
    registry.enqueue(ActionEntry{actions::Launch, {}, std::chrono::steady_clock::now() - std::chrono::seconds{30}});

    registry.finished(); // Finished processing the 1st action
    REQUIRE(registry.hasPendingAction());

    registry.finished();                   // Finished processing the 2nd action
    REQUIRE(!registry.hasPendingAction()); // 3rd action has expired.
}

TEST_CASE("Given actions registry when enqueue multiple actions at once then process the specific ones only.")
{
    int counter             = 0;
    auto nextActionCallback = [&counter](ActionEntry &entry) {
        if (entry.actionId == actions::UserAction) {
            ++counter;
            return ActionProcessStatus::Accepted;
        }
        return ActionProcessStatus::Skipped;
    };
    ActionsRegistry registry{std::move(nextActionCallback)};

    registry.enqueue(ActionEntry{actions::Launch});
    REQUIRE(!registry.hasPendingAction());

    registry.enqueue(ActionEntry{actions::UserAction});
    registry.enqueue(ActionEntry{actions::Launch});
    REQUIRE(counter == 1);

    registry.finished();
    REQUIRE(counter == 1);
    REQUIRE(!registry.hasPendingAction());
}

TEST_CASE("Process the specific actions only and skip others.")
{
    int acceptedCounter     = 0;
    int skippedCounter      = 0;
    int droppedCounter      = 0;
    auto nextActionCallback = [&acceptedCounter, &skippedCounter, &droppedCounter](ActionEntry &entry) {
        if (entry.actionId == actions::UserAction) {
            ++acceptedCounter;
            return ActionProcessStatus::Accepted;
        }
        if (entry.actionId == actions::Home) {
            ++skippedCounter;
            return ActionProcessStatus::Skipped;
        }
        ++droppedCounter;
        return ActionProcessStatus::Dropped;
    };
    ActionsRegistry registry{std::move(nextActionCallback)};

    registry.enqueue(ActionEntry{actions::Launch});
    REQUIRE(droppedCounter == 1);
    REQUIRE(!registry.hasPendingAction());

    registry.enqueue(ActionEntry{actions::Home});
    REQUIRE(skippedCounter == 1);
    REQUIRE(!registry.hasPendingAction());

    registry.enqueue(ActionEntry{actions::UserAction});
    registry.finished();
    REQUIRE(acceptedCounter == 1);
}
