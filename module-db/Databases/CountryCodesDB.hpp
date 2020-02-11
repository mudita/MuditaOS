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
