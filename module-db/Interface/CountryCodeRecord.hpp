#pragma once

#include "../Common/Common.hpp"
#include "../Databases/CountryCodesDB.hpp"
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
    {
    }
    ~CountryCodeRecordInterface()
    {
    }
    uint32_t GetCountryCodeByMCC(uint32_t mcc)
    {
        return (0);
    }

  private:
    CountryCodesDB *codesDB;
};
