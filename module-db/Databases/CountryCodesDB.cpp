// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CountryCodesDB.hpp"

CountryCodesDB::CountryCodesDB(const char *name) : Database(name, true), countryCodes(this)
{
    if (countryCodes.create() == false)
        return;
    isInitialized_ = true;
}

CountryCodesDB::~CountryCodesDB()
{}
