// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <module-apps/application-call/presenter/CallPresenter.hpp>
#include <module-apps/application-call/test/mock/CallPresenterMocks.hpp>
#include <module-apps/application-call/data/CallAppStyle.hpp>

using app::call::ViewMock;

TEST_CASE("Call presenter")
{
    SECTION("Show incoming call layout")
    {
        auto model     = std::make_shared<app::call::ModelMock>();
        auto presenter = app::call::CallWindowContract::Presenter(model);
        auto view      = app::call::ViewMock();
        presenter.attach(&view);

        model->setState(app::call::CallState::Incoming);
        presenter.handleCallStateChange();

        REQUIRE(view.windowRefreshed == true);
        REQUIRE(view.layoutShowed == ViewMock::LayoutShowed::Incoming);
        REQUIRE(view.duration == callAppStyle::strings::iscalling);
    }

    SECTION("Show call active layout")
    {
        auto model     = std::make_shared<app::call::ModelMock>();
        auto presenter = app::call::CallWindowContract::Presenter(model);
        auto view      = app::call::ViewMock();
        presenter.attach(&view);

        model->setState(app::call::CallState::Active);
        presenter.handleCallStateChange();

        REQUIRE(view.windowRefreshed == true);
        REQUIRE(view.layoutShowed == ViewMock::LayoutShowed::Active);
    }

    SECTION("Show call outgoing layout")
    {
        auto model     = std::make_shared<app::call::ModelMock>();
        auto presenter = app::call::CallWindowContract::Presenter(model);
        auto view      = app::call::ViewMock();
        presenter.attach(&view);

        model->setState(app::call::CallState::Outgoing);
        presenter.handleCallStateChange();

        REQUIRE(view.windowRefreshed == true);
        REQUIRE(view.layoutShowed == ViewMock::LayoutShowed::Active);
        REQUIRE(view.duration == callAppStyle::strings::calling);
    }

    SECTION("Show call ended layout")
    {
        auto model     = std::make_shared<app::call::ModelMock>();
        auto presenter = app::call::CallWindowContract::Presenter(model);
        auto view      = app::call::ViewMock();
        presenter.attach(&view);

        model->setState(app::call::CallState::Ended);
        presenter.handleCallStateChange();

        REQUIRE(view.windowRefreshed == true);
        REQUIRE(view.duration == callAppStyle::strings::callended);
    }

    SECTION("Show call rejected layout")
    {
        auto model     = std::make_shared<app::call::ModelMock>();
        auto presenter = app::call::CallWindowContract::Presenter(model);
        auto view      = app::call::ViewMock();
        presenter.attach(&view);

        model->setState(app::call::CallState::Rejected);
        presenter.handleCallStateChange();

        REQUIRE(view.windowRefreshed == true);
        REQUIRE(view.layoutShowed == ViewMock::LayoutShowed::Ended);
        REQUIRE(view.duration == callAppStyle::strings::callrejected);
    }

    SECTION("Handle left button for incoming call")
    {
        auto model     = std::make_shared<app::call::ModelMock>();
        auto presenter = app::call::CallWindowContract::Presenter(model);

        model->setState(app::call::CallState::Incoming);
        auto result = presenter.handleLeftButton();

        REQUIRE(result == true);
        REQUIRE(model->answerCallCalled == true);
    }

    SECTION("Handle left button for not incoming call")
    {
        auto model     = std::make_shared<app::call::ModelMock>();
        auto presenter = app::call::CallWindowContract::Presenter(model);

        model->setState(app::call::CallState::None);
        auto result = presenter.handleLeftButton();

        REQUIRE(result == false);
        REQUIRE(model->answerCallCalled == false);

        model->answerCallCalled = false;
        model->setState(app::call::CallState::Outgoing);
        result = presenter.handleLeftButton();

        REQUIRE(result == false);

        model->answerCallCalled = false;
        model->setState(app::call::CallState::Active);
        result = presenter.handleLeftButton();

        REQUIRE(result == false);
        REQUIRE(model->answerCallCalled == false);

        model->answerCallCalled = false;
        model->setState(app::call::CallState::Ended);
        result = presenter.handleLeftButton();

        REQUIRE(result == false);
        REQUIRE(model->answerCallCalled == false);

        model->answerCallCalled = false;
        model->setState(app::call::CallState::Rejected);
        result = presenter.handleLeftButton();

        REQUIRE(result == false);
        REQUIRE(model->answerCallCalled == false);
    }

    SECTION("Handle right button for incoming call")
    {
        auto model     = std::make_shared<app::call::ModelMock>();
        auto presenter = app::call::CallWindowContract::Presenter(model);

        model->setState(app::call::CallState::Incoming);
        auto result = presenter.handleRightButton();

        REQUIRE(result == true);
        REQUIRE(model->hangupCallCalled == true);
    }

    SECTION("Handle right button for active call")
    {
        auto model     = std::make_shared<app::call::ModelMock>();
        auto presenter = app::call::CallWindowContract::Presenter(model);

        model->setState(app::call::CallState::Active);
        auto result = presenter.handleRightButton();

        REQUIRE(result == true);
        REQUIRE(model->hangupCallCalled == true);

        model->hangupCallCalled = false;
        model->setState(app::call::CallState::Outgoing);
        result = presenter.handleRightButton();

        REQUIRE(result == true);
        REQUIRE(model->hangupCallCalled == true);
    }

    SECTION("Handle right button for not active call")
    {
        auto model     = std::make_shared<app::call::ModelMock>();
        auto presenter = app::call::CallWindowContract::Presenter(model);

        model->setState(app::call::CallState::None);
        auto result = presenter.handleRightButton();

        REQUIRE(result == false);
        REQUIRE(model->hangupCallCalled == false);

        model->hangupCallCalled = false;
        model->setState(app::call::CallState::Ended);
        result = presenter.handleRightButton();

        REQUIRE(result == false);
        REQUIRE(model->hangupCallCalled == false);

        model->hangupCallCalled = false;
        model->setState(app::call::CallState::Rejected);
        result = presenter.handleRightButton();

        REQUIRE(result == false);
        REQUIRE(model->hangupCallCalled == false);
    }

    SECTION("Handle headset ok for incoming call")
    {
        auto model     = std::make_shared<app::call::ModelMock>();
        auto presenter = app::call::CallWindowContract::Presenter(model);

        model->setState(app::call::CallState::Incoming);
        auto result = presenter.handleHeadsetOk();

        REQUIRE(result == true);
        REQUIRE(model->answerCallCalled == true);
    }

    SECTION("Handle headset ok active call")
    {
        auto model     = std::make_shared<app::call::ModelMock>();
        auto presenter = app::call::CallWindowContract::Presenter(model);

        model->setState(app::call::CallState::Outgoing);
        auto result = presenter.handleHeadsetOk();

        REQUIRE(result == true);
        REQUIRE(model->hangupCallCalled == true);

        model->hangupCallCalled = false;
        model->setState(app::call::CallState::Active);
        result = presenter.handleHeadsetOk();

        REQUIRE(result == true);
        REQUIRE(model->hangupCallCalled == true);
    }

    SECTION("Handle headset ok for not active call")
    {
        auto model     = std::make_shared<app::call::ModelMock>();
        auto presenter = app::call::CallWindowContract::Presenter(model);

        model->setState(app::call::CallState::None);
        auto result = presenter.handleHeadsetOk();

        REQUIRE(result == false);
        REQUIRE(model->hangupCallCalled == false);
        REQUIRE(model->answerCallCalled == false);

        model->hangupCallCalled = false;
        model->setState(app::call::CallState::Ended);
        result = presenter.handleHeadsetOk();

        REQUIRE(result == false);
        REQUIRE(model->hangupCallCalled == false);
        REQUIRE(model->answerCallCalled == false);
    }
}
