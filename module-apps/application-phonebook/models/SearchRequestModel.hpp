// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
