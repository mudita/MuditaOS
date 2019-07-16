/*
 * AlarmsTable.cpp
 *
 *  Created on: 15 lip 2019
 *      Author: kuba
 */

#include "AlarmsTable.hpp"


AlarmsTable::AlarmsTable(Database *db) : Table(db) {
}

AlarmsTable::~AlarmsTable() {

}

bool AlarmsTable::Create() {
    return db->Execute(createTableQuery);
}

bool AlarmsTable::Add(AlarmsTableRow entry) {
    return db->Execute(
            "INSERT or ignore INTO alarms ( time, snooze, status, path ) VALUES (%lu,%lu,%lu,'%s');",
            entry.time,
			entry.snooze,
			entry.status,
			entry.path.c_str()
    );
}

bool AlarmsTable::RemoveByID(uint32_t id) {
    return db->Execute("DELETE FROM alarms where _id = %u;", id);
}

bool AlarmsTable::RemoveByField(AlarmsTableFields field, const char *str) {
    std::string fieldName;

    switch(field){
        case AlarmsTableFields ::Time:
            fieldName = "time";
            break;

        case AlarmsTableFields ::Snooze:
            fieldName = "snooze";
            break;

        case AlarmsTableFields ::Status:
            fieldName = "status";
            break;
        default:
            return false;
    }

    return db->Execute("DELETE FROM alarms where %s = '%s';", fieldName.c_str(),str);

}


bool AlarmsTable::Update(AlarmsTableRow entry) {
    return db->Execute(
            "UPDATE alarms SET time = %lu, snooze = %lu ,status = %lu, path = '%s' WHERE _id=%lu;",
            entry.time,
			entry.snooze,
			entry.status,
			entry.path,
            entry.ID
    );
}

AlarmsTableRow AlarmsTable::GetByID(uint32_t id) {
    auto retQuery = db->Query("SELECT * FROM alarms WHERE _id= %u;", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return AlarmsTableRow();
    }

    return AlarmsTableRow{(*retQuery)[0].GetUInt32(),  // ID
                       (*retQuery)[1].GetUInt32(),    // time
                       (*retQuery)[2].GetUInt32(),    // snooze
                       (*retQuery)[3].GetUInt32(),    // status
                       (*retQuery)[6].GetString(),    // path
    };
}

std::vector<AlarmsTableRow> AlarmsTable::GetLimitOffset(uint32_t offset, uint32_t limit) {
    auto retQuery = db->Query("SELECT * from sms ORDER BY date LIMIT %lu OFFSET %lu;",
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<AlarmsTableRow>();
    }

    std::vector<AlarmsTableRow> ret;

    do {
        ret.push_back(AlarmsTableRow{(*retQuery)[0].GetUInt32(),  // ID
            							(*retQuery)[1].GetUInt32(),    // time
										(*retQuery)[2].GetUInt32(),    // snooze
										(*retQuery)[3].GetUInt32(),    // status
										(*retQuery)[6].GetString(),    // path
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<AlarmsTableRow>
AlarmsTable::GetLimitOffsetByField(uint32_t offset, uint32_t limit, AlarmsTableFields field, const char *str) {

    std::string fieldName;
    switch (field) {
        case AlarmsTableFields::Time:
            fieldName = "time";
            break;
        case AlarmsTableFields ::Snooze:
            fieldName = "snooze";
            break;
        case AlarmsTableFields ::Status:
            fieldName = "status";
            break;
        default:
            return std::vector<AlarmsTableRow>();
    }

    auto retQuery = db->Query("SELECT * from sms WHERE %s='%s' ORDER BY date LIMIT %lu OFFSET %lu;",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<AlarmsTableRow>();
    }

    std::vector<AlarmsTableRow> ret;

    do {
    	 ret.push_back(AlarmsTableRow{(*retQuery)[0].GetUInt32(),  // ID
										(*retQuery)[1].GetUInt32(),    // time
										(*retQuery)[2].GetUInt32(),    // snooze
										(*retQuery)[3].GetUInt32(),    // status
										(*retQuery)[6].GetString(),    // path
    	        });
    } while (retQuery->NextRow());

    return ret;
}

uint32_t AlarmsTable::GetCount() {
    auto queryRet = db->Query("SELECT COUNT(*) FROM alarms;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t AlarmsTable::GetCountByFieldID(const char *field, uint32_t id) {
    auto queryRet = db->Query("SELECT COUNT(*) FROM alarms WHERE %s=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}



