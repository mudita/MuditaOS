// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WindowsPopupFilter.hpp"
#include <catch2/catch.hpp>
#include <module-apps/apps-common/WindowsPopupQueue.hpp>

gui::popup::Filter filter;

TEST_CASE("WindowsPopupQueue creation")
{
    auto wp = app::WindowsPopupQueue();

    std::optional<gui::popup::Request> r = wp.popRequest(filter);
    REQUIRE(not r);
}

TEST_CASE("WindowsPopupQueue push")
{
    using namespace gui::popup;
    auto wp = app::WindowsPopupQueue();
    Blueprint blueprint;
    SECTION("single")
    {
        Request request(
            gui::popup::ID::Alarm, std::make_unique<gui::PopupRequestParams>(gui::popup::ID::Alarm), blueprint);
        wp.pushRequest(std::move(request));
        wp.popRequest(filter);
        auto r = wp.popRequest(filter);
        REQUIRE(not r);
    }

    /// we should be able to have multiple same requests stored on the popups queue
    SECTION("multiple")
    {
        const auto count = 3;
        auto request_gen = [blueprint]() {
            return Request(
                gui::popup::ID::Alarm, std::make_unique<gui::PopupRequestParams>(gui::popup::ID::Alarm), blueprint);
        };
        Request request[count] = {request_gen(), request_gen(), request_gen()};
        for (auto &val : request) {
            wp.pushRequest(std::move(val));
        }
        for (auto i = count; i != 0; --i) {
            auto r = wp.popRequest(filter);
            REQUIRE(r);
        }
    }
}

class NoneFilter : public gui::popup::Filter
{
    bool isPermitted(const gui::PopupRequestParams & /*params*/) const override
    {
        return false;
    }
};

TEST_CASE("WindowsPopupQueue filter")
{
    using namespace gui::popup;
    auto wp = app::WindowsPopupQueue();
    Blueprint blueprint;
    auto noneFilter = NoneFilter();
    Request request(gui::popup::ID::Alarm, std::make_unique<gui::PopupRequestParams>(gui::popup::ID::Alarm), blueprint);
    wp.pushRequest(std::move(request));
    {
        auto r = wp.popRequest(noneFilter);
        REQUIRE(not r);
    }
    {
        auto r = wp.popRequest(filter);
        REQUIRE(r);
    }
    {
        auto r = wp.popRequest(noneFilter);
        REQUIRE(not r);
    }
    {
        auto r = wp.popRequest(filter);
        REQUIRE(not r);
    }
}

TEST_CASE("WindowsPopupRequest - handle empty request foo")
{
    using namespace gui::popup;
    SECTION("no blueprint")
    {
        Blueprint blueprint;
        Request request(
            gui::popup::ID::Alarm, std::make_unique<gui::PopupRequestParams>(gui::popup::ID::Alarm), blueprint);
        REQUIRE_THROWS_AS(request.handle(), std::bad_function_call);
    }

    SECTION("failing blueprint")
    {
        Blueprint blueprint = [](gui::popup::ID, std::unique_ptr<gui::PopupRequestParams> &) -> bool { return false; };
        Request request(
            gui::popup::ID::Alarm, std::make_unique<gui::PopupRequestParams>(gui::popup::ID::Alarm), blueprint);
        REQUIRE(not request.handle());
    }

    SECTION("successful blueprint")
    {
        Blueprint blueprint = [](gui::popup::ID, std::unique_ptr<gui::PopupRequestParams> &) -> bool { return true; };
        Request request(
            gui::popup::ID::Alarm, std::make_unique<gui::PopupRequestParams>(gui::popup::ID::Alarm), blueprint);
        REQUIRE(request.handle());
    }
}
