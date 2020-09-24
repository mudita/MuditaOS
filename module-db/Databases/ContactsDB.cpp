
#include <module-vfs/vfs.hpp>
#include "ContactsDB.hpp"

const char *ContactsDB::dbName = USER_PATH("contacts.db");

uint32_t ContactsDB::favouritesId = 0;
uint32_t ContactsDB::iceId        = 0;
uint32_t ContactsDB::blockedId    = 0;
uint32_t ContactsDB::temporaryId  = 0;

ContactsDB::ContactsDB()
    : Database(dbName), contacts(this), name(this), number(this), ringtones(this), address(this), groups(this)
{

    if (contacts.create() == false) {
        return;
    }
    if (name.create() == false) {
        return;
    }
    if (number.create() == false) {
        return;
    }
    if (ringtones.create() == false) {
        return;
    }
    if (address.create() == false) {
        return;
    }
    if (groups.create() == false) {
        return;
    }

    if (favouritesId == 0) {
        favouritesId = groups.favouritesId();
    }
    if (iceId == 0) {
        iceId = groups.iceId();
    }
    if (blockedId == 0) {
        blockedId = groups.blockedId();
    }
    if (temporaryId == 0) {
        temporaryId = groups.temporaryId();
    }

    isInitialized_ = true;
}

ContactsDB::~ContactsDB()
{}
