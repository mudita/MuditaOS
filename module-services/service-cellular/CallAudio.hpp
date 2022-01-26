// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace sys
{
    class Service;
}

class CallRingAudio
{
    sys::Service &owner;

  public:
    explicit CallRingAudio(sys::Service &);

    void play();
    void stop();
};
