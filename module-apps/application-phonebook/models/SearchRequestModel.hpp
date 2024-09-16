// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

class SearchRequestModel
{
    bool searchRequested = false;

  public:
    bool requestedSearch() const noexcept
    {
        return searchRequested;
    }
    void setRequested(bool val) noexcept
    {
        searchRequested = val;
    }
};
