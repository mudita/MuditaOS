// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ContactsNumberTable.hpp"
#include "Common/Types.hpp"

ContactsNumberTable::ContactsNumberTable(Database *db) : Table(db)
{}

ContactsNumberTable::~ContactsNumberTable()
{}

bool ContactsNumberTable::create()
{
    return true;
}

bool ContactsNumberTable::add(ContactsNumberTableRow entry)
{
    return db->execute(
        "insert or ignore into contact_number (contact_id, number_user, number_e164, type) VALUES (" u32_c str_c str_c
            u32_ ");",
        entry.contactID,
        entry.numberUser.c_str(),
        entry.numbere164.c_str(),
        entry.type);
}

bool ContactsNumberTable::removeById(uint32_t id)
{
    return db->execute("DELETE FROM contact_number where _id=" u32_ ";", id);
}

bool ContactsNumberTable::update(ContactsNumberTableRow entry)
{
    return db->execute("UPDATE contact_number SET contact_id=" u32_c "number_user=" str_c "number_e164=" str_c
                       "type=" u32_ " WHERE _id=" u32_ ";",
                       entry.contactID,
                       entry.numberUser.c_str(),
                       entry.numbere164.c_str(),
                       entry.type,
                       entry.ID);
}

ContactsNumberTableRow ContactsNumberTable::getById(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM contact_number WHERE _id=" u32_ ";", id);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return ContactsNumberTableRow();
    }

    return ContactsNumberTableRow{
        (*retQuery)[0].getUInt32(),                                 // ID
        (*retQuery)[1].getUInt32(),                                 // contactID
        (*retQuery)[2].getString(),                                 // numberUser
        (*retQuery)[3].getString(),                                 // numbere164
        static_cast<ContactNumberType>((*retQuery)[4].getUInt32()), // type
    };
}

std::vector<ContactsNumberTableRow> ContactsNumberTable::getByContactId(uint32_t id)
{
    auto retQuery = db->query("SELECT * FROM contact_number WHERE contact_id=" u32_ ";", id);
    if (!retQuery || (retQuery->getRowCount() == 0U)) {
        return {};
    }

    std::vector<ContactsNumberTableRow> ret;
    ret.reserve(retQuery->getRowCount());
    do {
        ContactsNumberTableRow row{
            (*retQuery)[0].getUInt32(),                                 // ID
            (*retQuery)[1].getUInt32(),                                 // contactID
            (*retQuery)[2].getString(),                                 // numberUser
            (*retQuery)[3].getString(),                                 // numbere164
            static_cast<ContactNumberType>((*retQuery)[4].getUInt32()), // type
        };
        ret.push_back(std::move(row));
    } while (retQuery->nextRow());
    return ret;
}

std::vector<ContactsNumberTableRow> ContactsNumberTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->query("SELECT * from contact_number LIMIT " u32_ " OFFSET " u32_ ";", limit, offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsNumberTableRow>();
    }

    std::vector<ContactsNumberTableRow> ret;

    do {
        ret.push_back(ContactsNumberTableRow{
            (*retQuery)[0].getUInt32(),                                 // ID
            (*retQuery)[1].getUInt32(),                                 // contactID
            (*retQuery)[2].getString(),                                 // numberUser
            (*retQuery)[3].getString(),                                 // numbere164
            static_cast<ContactNumberType>((*retQuery)[4].getUInt32()), // type
        });
    } while (retQuery->nextRow());

    return ret;
}

std::vector<ContactsNumberTableRow> ContactsNumberTable::getLimitOffset(const std::string &number,
                                                                        uint32_t offset,
                                                                        uint32_t limit)
{
    const char lastCharacter = number.back();
    auto retQuery =
        db->query("SELECT * from contact_number WHERE number_user like '%%%c' LIMIT " u32_ " OFFSET " u32_ ";",
                  lastCharacter,
                  limit,
                  offset);
    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsNumberTableRow>();
    }

    std::vector<ContactsNumberTableRow> ret;
    do {
        ret.push_back(ContactsNumberTableRow{
            (*retQuery)[0].getUInt32(),                                 // ID
            (*retQuery)[1].getUInt32(),                                 // contactID
            (*retQuery)[2].getString(),                                 // numberUser
            (*retQuery)[3].getString(),                                 // numbere164
            static_cast<ContactNumberType>((*retQuery)[4].getUInt32()), // type
        });
    } while (retQuery->nextRow());
    return ret;
}

std::vector<ContactsNumberTableRow> ContactsNumberTable::getLimitOffsetByField(uint32_t offset,
                                                                               uint32_t limit,
                                                                               ContactNumberTableFields field,
                                                                               const char *str)
{
    std::string fieldName;
    switch (field) {
    case ContactNumberTableFields::NumberUser:
        fieldName = "number_user";
        break;
    case ContactNumberTableFields::NumberE164:
        fieldName = "number_e164";
        break;
    default:
        return std::vector<ContactsNumberTableRow>();
    }

    auto retQuery =
        db->query("SELECT * from contact_number WHERE %q=" str_ " ORDER BY number_user LIMIT " u32_ " OFFSET " u32_ ";",
                  fieldName.c_str(),
                  str,
                  limit,
                  offset);

    if ((retQuery == nullptr) || (retQuery->getRowCount() == 0)) {
        return std::vector<ContactsNumberTableRow>();
    }

    std::vector<ContactsNumberTableRow> ret;

    do {
        ret.push_back(ContactsNumberTableRow{
            (*retQuery)[0].getUInt32(),                                 // ID
            (*retQuery)[1].getUInt32(),                                 // contactID
            (*retQuery)[2].getString(),                                 // numberUser
            (*retQuery)[3].getString(),                                 // numbere164
            static_cast<ContactNumberType>((*retQuery)[4].getUInt32()), // type
        });
    } while (retQuery->nextRow());

    return ret;
}

uint32_t ContactsNumberTable::count()
{
    auto queryRet = db->query("SELECT COUNT(*) FROM contact_number;");

    if (!queryRet || queryRet->getRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}

uint32_t ContactsNumberTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->query("SELECT COUNT(*) FROM contact_number WHERE %q=" u32_ ";", field, id);

    if ((queryRet == nullptr) || (queryRet->getRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].getUInt32()};
}
