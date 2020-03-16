#include "CountryCodesTable.hpp"

CountryCodesTable::CountryCodesTable(Database *db) : Table(db)
{}

CountryCodesTable::~CountryCodesTable()
{}

bool CountryCodesTable::Create()
{
    return db->Execute(createTableQuery);
}

CodesTableRow CountryCodesTable::GetByMCC(uint32_t mcc)
{
    auto retQuery = db->Query("SELECT * FROM codes WHERE mcc= %lu LIMIT 1;", mcc);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return CodesTableRow();
    }

    return CodesTableRow{
        (*retQuery)[0].GetUInt32(), /* _id */
        (*retQuery)[1].GetUInt32(), /* mcc */
        (*retQuery)[2].GetUInt32(), /* mnc */
        (*retQuery)[3].GetString(), /* iso */
        (*retQuery)[4].GetString(), /* country name */
        (*retQuery)[5].GetUInt32(), /* country code */
        (*retQuery)[5].GetString()  /* network name */
    };
}

uint32_t CountryCodesTable::GetCount()
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM codes;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}
