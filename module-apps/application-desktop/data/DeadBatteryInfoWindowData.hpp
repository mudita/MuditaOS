// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <SwitchData.hpp>

class DeadBatteryInfoWindowData : public gui::SwitchData
{
  public:
    explicit DeadBatteryInfoWindowData(const std::string &message) : message(message)
    {}

    std::string getMessageText()
    {
        return message;
    }

  private:
    const std::string message;
};
