#include "CountryCodesDB.hpp"

const char *CountryCodesDB::dbName = "country-codes.db";

CountryCodesDB::CountryCodesDB() : Database(dbName), countryCodes(this)
{
    if (countryCodes.Create() == false)
        return;
    isInitialized = true;
}

CountryCodesDB::~CountryCodesDB()
{}