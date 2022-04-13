// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace sys
{
    class Service;
}

namespace call::api
{
    class Audio
    {
      public:
        virtual void play()              = 0;
        virtual void stop()              = 0;
        virtual void muteCall()          = 0;
        virtual void unmuteCall()        = 0;
        virtual void setLaudspeakerOn()  = 0;
        virtual void setLaudspeakerOff() = 0;
        virtual void routingStart()      = 0;
        virtual ~Audio()                 = default;
    };
} // namespace call::api

class Audio : public call::api::Audio
{
    struct CallMeta;
    sys::Service *owner;
    CallMeta *meta = nullptr;
    bool started   = false;

  public:
    explicit Audio(sys::Service *);
    ~Audio();

    void play();
    void stop();
    void muteCall();
    void unmuteCall();
    void setLaudspeakerOn();
    void setLaudspeakerOff();
    void routingStart();
};
