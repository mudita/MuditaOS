// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-desktop/DesktopMessages.hpp"
#include "parser/MessageHandler.hpp"
#include <Device.hpp>
#include <EventStore.hpp>
#include <source/version.hpp>
#include <sys/statvfs.h>

namespace sdesktop
{
    using namespace parserFSM;
    namespace developerMode
    {

        AppFocusChangeEvent::AppFocusChangeEvent(std::string appName)
        {
            context.setResponseStatus(http::Code::OK);
            context.setEndpoint(EndpointType::developerMode);
            context.setResponseBody(json11::Json::object{{json::developerMode::focus, appName}});
        }

        ScreenlockCheckEvent::ScreenlockCheckEvent(bool isLocked)
        {
            context.setResponseStatus(http::Code::OK);
            context.setEndpoint(EndpointType::developerMode);
            context.setResponseBody(json11::Json::object{{json::developerMode::phoneLocked, isLocked}});
        }

        CellularStateInfoRequestEvent::CellularStateInfoRequestEvent(std::string stateStr)
        {
            context.setResponseStatus(http::Code::OK);
            context.setEndpoint(EndpointType::developerMode);
            context.setResponseBody(json11::Json::object{{json::developerMode::cellularStateInfo, stateStr}});
        }

        CellularSleepModeInfoRequestEvent::CellularSleepModeInfoRequestEvent(bool isInSleepMode)
        {
            context.setResponseStatus(http::Code::OK);
            context.setEndpoint(EndpointType::developerMode);
            context.setResponseBody(json11::Json::object{{json::developerMode::cellularSleepModeInfo, isInSleepMode}});
        }
    } // namespace developerMode

    DeviceInfoRequestEvent::DeviceInfoRequestEvent(const std::string &networkOperatorName)
    {
        // json11::Json updateHistory   = static_cast<ServiceDesktop
        // *>(ownerServicePtr)->updateOS->getUpdateHistory();
        std::unique_ptr<struct statvfs> vfstat = std::make_unique<struct statvfs>();
        if ((statvfs)(purefs::dir::getRootDiskPath().c_str(), vfstat.get()) < 0) {
            return;
        }
        unsigned long totalMbytes = (vfstat->f_frsize * vfstat->f_blocks) / 1024LLU / 1024LLU;
        unsigned long freeMbytes  = (vfstat->f_bfree * vfstat->f_bsize) / 1024LLU / 1024LLU;
        unsigned long freePercent = (freeMbytes * 100) / totalMbytes;

        context.setResponseStatus(http::Code::OK);
        context.setEndpoint(EndpointType::deviceInfo);
        context.setResponseBody(json11::Json::object(
            {{json::batteryLevel, std::to_string(Store::Battery::get().level)},
             {json::batteryState, std::to_string(static_cast<int>(Store::Battery::get().state))},
             {json::selectedSim, std::to_string(static_cast<int>(Store::GSM::get()->selected))},
             {json::trayState, std::to_string(static_cast<int>(Store::GSM::get()->tray))},
             {json::signalStrength, std::to_string(static_cast<int>(Store::GSM::get()->getSignalStrength().rssiBar))},
             {json::accessTechnology,
              std::to_string(static_cast<int>(Store::GSM::get()->getNetwork().accessTechnology))},
             {json::networkStatus, std::to_string(static_cast<int>(Store::GSM::get()->getNetwork().status))},
             {json::networkOperatorName, networkOperatorName},
             {json::fsTotal, std::to_string(totalMbytes)},
             {json::fsFree, std::to_string(freeMbytes)},
             {json::fsFreePercent, std::to_string(freePercent)},
             {json::gitRevision, (std::string)(GIT_REV)},
             {json::gitTag, (std::string)GIT_TAG},
             {json::gitBranch, (std::string)GIT_BRANCH},
             //{json::updateHistory, updateHistory},
             {json::currentRTCTime, std::to_string(static_cast<uint32_t>(std::time(nullptr)))},
             {json::version, std::string(VERSION)}}));
    }

    namespace usb
    {
        USBConfigured::USBConfigured(USBConfigurationType configurationType)
            : sys::DataMessage(MessageType::USBConfigured), configurationType(configurationType)
        {}

        USBConfigurationType USBConfigured::getConfigurationType() const noexcept
        {
            return configurationType;
        }
    } // namespace usb

} // namespace sdesktop
