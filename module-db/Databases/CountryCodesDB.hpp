// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Database/Database.hpp"
#include "Tables/CountryCodesTable.hpp"

class CountryCodesDB : public Database
{
  public:
    CountryCodesDB();
    ~CountryCodesDB();

    static const char *GetDBName()
    {
        return dbName;
    }

    CountryCodesTable countryCodes;

  private:
    static const char *dbName;
};
