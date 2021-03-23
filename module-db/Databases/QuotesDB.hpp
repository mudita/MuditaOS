// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Database/Database.hpp"
#include "../Tables/QuotesTable.hpp"

class QuotesDB : public Database
{
  public:
    QuotesDB(const char *name);
    ~QuotesDB() = default;

    QuotesTable quotes;
};
