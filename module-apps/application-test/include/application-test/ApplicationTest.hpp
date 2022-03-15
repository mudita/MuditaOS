// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <SwitchData.hpp>
#include <AppWindow.hpp>

namespace app::name
{
    constexpr auto test = "test-app";
}

namespace gui
{
    class TestPresenter;
};

namespace app
{

    inline constexpr auto application_test = "ApplicationTest";

    class ApplicationTest : public app::Application
    {
      public:
        explicit ApplicationTest(const std::string &name             = application_test,
                                 const std::string &parent           = {},
                                 StatusIndicators statusIndicators   = StatusIndicators{},
                                 StartInBackground startInBackground = {false});
        // NOTE Parameter above will mess up your action logic hard if you want to Launch app
        /// required for forward declaraion of unique_ptr<TestPresenter>
        ~ApplicationTest();

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode /*mode*/) override final
        {
            return sys::ReturnCodes::Success;
        }
        void createUserInterface() override;
        void destroyUserInterface() override;

      private:
        std::unique_ptr<gui::TestPresenter> presenter;
    };

    template <> struct ManifestTraits<ApplicationTest>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {// enable application to show (any) popup registered in it
                    {manager::actions::Launch, manager::actions::ShowPopup},
                    // this application forbids AutoLock
                    // it's solely for PurePhone
                    // NOTE - documentation needed here
                    locks::AutoLockPolicy::PreventPermanently};
        };
    };
} // namespace app
