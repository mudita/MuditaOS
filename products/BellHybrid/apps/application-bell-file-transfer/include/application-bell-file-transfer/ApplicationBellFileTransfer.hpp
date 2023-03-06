// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

namespace gui::window::name
{} // namespace gui::window::name

namespace app
{
    inline constexpr auto applicationBellFileTransferName = "ApplicationBellFileTransfer";

    class ApplicationBellFileTransfer : public Application
    {
        //      private:
        //        void onStop() override;
        //        sys::MessagePointer handleSwitchWindow(sys::Message *msgl) override;

      public:
        ApplicationBellFileTransfer(std::string name                    = applicationBellFileTransferName,
                                    std::string parent                  = "",
                                    StatusIndicators statusIndicators   = StatusIndicators{},
                                    StartInBackground startInBackground = {false},
                                    uint32_t stackDepth                 = 4096 * 2);
        virtual ~ApplicationBellFileTransfer();
        sys::ReturnCodes InitHandler() override;

        void createUserInterface() override;
        void destroyUserInterface() override
        {}
        sys::MessagePointer handleSwitchWindow(sys::Message *msgl) override;

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }
    };

    template <>
    struct ManifestTraits<ApplicationBellFileTransfer>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app