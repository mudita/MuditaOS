// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "../Common/Common.hpp"
#include "module-db/databases/CountryCodesDB.hpp"
#include "Record.hpp"
#include "utf8/UTF8.hpp"

#include <vector>

struct CountryCodeRecord
{
    uint32_t id;
    uint32_t mcc;
    uint32_t mnc;
    UTF8 iso;
    UTF8 country;
    uint32_t country_code;
    UTF8 network;
};

enum class CountryCodeRecordField
{
    MCC,
    MNC,
    ISO,
    Country
};

class CountryCodeRecordInterface : public RecordInterface<CountryCodeRecord, CountryCodeRecordField>
{

  public:
    CountryCodeRecordInterface(CountryCodesDB *db) : codesDB(db)
    {}
    ~CountryCodeRecordInterface()
    {}
    CodesTableRow GetByMCC(uint32_t mcc)
    {
        if (codesDB) {
            return (codesDB->countryCodes.GetByMCC(mcc));
        }

        return (CodesTableRow());
    }

  private:
    CountryCodesDB *codesDB;
};
