// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "RTCCommandInterface.hpp"

class RTCCommand : public RTCCommandInterface
{
  public:
    explicit RTCCommand(sys::Service *owner) : owner(owner)
    {}
    virtual ~RTCCommand() = default;
    virtual void setTime(const struct tm &time) final;
    virtual void setTime(const time_t &time) final;
    virtual void setTimezone(const std::string &timezone) final;

  private:
    sys::Service *owner = nullptr;
};
