// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/ServiceForward.hpp>

namespace bluetooth
{
    class CellularInterface
    {
      public:
        virtual ~CellularInterface()                           = default;
        virtual bool answerIncomingCall(sys::Service *service) = 0;
        virtual bool hangupCall(sys::Service *service)         = 0;
    };

    class CellularInterfaceImpl : public CellularInterface
    {
      public:
        bool answerIncomingCall(sys::Service *service) override;
        bool hangupCall(sys::Service *service) override;

      private:
        bool callActive = false;
    };

    class AudioInterface
    {
      public:
        virtual ~AudioInterface()                             = default;
        virtual bool startAudioRouting(sys::Service *service) = 0;
    };
    class AudioInterfaceImpl : public AudioInterface
    {
      public:
        bool startAudioRouting(sys::Service *service) override;
    };
} // namespace bluetooth
