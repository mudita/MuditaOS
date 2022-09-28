// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/ServiceForward.hpp>
#include <module-services/service-cellular/DTMFCode.hpp>

namespace bluetooth
{
    class CellularInterface
    {
      public:
        virtual ~CellularInterface()                                              = default;
        virtual bool answerIncomingCall(sys::Service *service)                    = 0;
        virtual bool hangupCall(sys::Service *service)                            = 0;
        virtual bool sendDTMFCode(sys::Service *service, DTMFCode code)           = 0;
        virtual bool dialNumber(sys::Service *service, const std::string &number) = 0;
    };

    class CellularInterfaceImpl : public CellularInterface
    {
      public:
        bool answerIncomingCall(sys::Service *service) override;
        bool hangupCall(sys::Service *service) override;
        bool dialNumber(sys::Service *service, const std::string &number) override;
        bool sendDTMFCode(sys::Service *service, DTMFCode code) override;
    };

    class AudioInterface
    {
      public:
        virtual ~AudioInterface()                             = default;
        virtual bool startAudioRouting(sys::Service *service) = 0;
        virtual bool stopAudioRouting(sys::Service *service)  = 0;
    };
    class AudioInterfaceImpl : public AudioInterface
    {
      public:
        bool startAudioRouting(sys::Service *service) override;
        bool stopAudioRouting(sys::Service *service) override;
    };
} // namespace bluetooth
