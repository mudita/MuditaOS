// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace sys
{
    class Service;
}

class CallRingAudio
{
    struct CallMeta;
    sys::Service &owner;
    CallMeta *meta = nullptr;
    bool started   = false;

  public:
    explicit CallRingAudio(sys::Service &);
    ~CallRingAudio();

    void play();
    void stop();
};
