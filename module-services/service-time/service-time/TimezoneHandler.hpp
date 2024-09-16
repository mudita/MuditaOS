// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <chrono>
#include <string>

class TimezoneHandler
{
  public:
    /**
     * Constructor. Creates custom timezone.
     * @param offset timezone offset related to UTC time
     */
    explicit TimezoneHandler(std::chrono::minutes offset) : offset(offset){};
    /**
     * Creates custom timezone variable
     * @return custom timezone
     */
    [[nodiscard]] auto getTimezone() -> std::string const;

  private:
    std::chrono::minutes offset;
};
