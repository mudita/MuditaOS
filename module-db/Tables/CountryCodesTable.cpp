// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CountryCodesTable.hpp"

CountryCodesTable::CountryCodesTable(Database *db) : Table(db)
{}

CountryCodesTable::~CountryCodesTable()
{}

bool CountryCodesTable::create()
{
    return db->execute(createTableQuery);
}

CodesTableRow CountryCodesTable::GetByMCC(uint32_t mcc)
{
    auto retQuery = db->query("SELECT * FROM codes WHERE mcc= %lu LIMIT 1;", mcc);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return CodesTableRow();
    }

    return CodesTableRow{
        (*retQuery)[0].getUInt32(), /* _id */
        (*retQuery)[1].getUInt32(), /* mcc */
        (*retQuery)[2].getUInt32(), /* mnc */
        (*retQuery)[3].getString(), /* iso */
        (*retQuery)[4].getString(), /* country name */
        (*retQuery)[5].getUInt32(), /* country code */
        (*retQuery)[5].getString()  /* network name */
    };
}

uint32_t CountryCodesTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM codes;");

    if (!queryRet || queryRet->getRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}
