#include "ContactListModel.hpp"

ContactListModel::ContactListModel(app::Application *app, uint32_t max_visible_el) : application(app) {}

bool ContactListModel::updateMax()
{
    bool ret = true;
    if (application != nullptr) {
        ret &= pos_favourites.max_index = DBServiceAPI::ContactGetCount(application);
        ret &= pos_contacts.max_index = DBServiceAPI::ContactGetCount(application);
    } else {
        ret = false;
    }
    return ret;
}

bool ContactListModel::all() { return pos_contacts.all(); }

void ContactListModel::updateLastCount(ContactType type, uint32_t val)
{
    if (type == ContactType::Favouirte) {
        pos_favourites.viewed_count = val;
    } else {
        pos_contacts.viewed_count = val;
    }
}

void ContactListModel::updateLastCount(uint32_t favourite_contacts_shown_cnt, uint32_t contacts_shown_cnt)
{
    pos_favourites.viewed_count = favourite_contacts_shown_cnt;
    pos_contacts.viewed_count = contacts_shown_cnt;
    LOG_INFO("Model: %d %d", pos_favourites.viewed_count, pos_contacts.viewed_count);
}

std::vector<ContactRecord> *ContactListModel::getContacts(ContactType type, uint32_t max_num)
{
    updateMax();
    if (type == Favouirte && !pos_favourites.all()) {
        // we need to reload?
        if (db_favourites == nullptr) {
            db_favourites = DBServiceAPI::ContactGetLimitOffsetByFavourites(application, pos_favourites.actual_index, max_num);
            pos_favourites.viewed_count = 0;
            LOG_INFO("Get contacts: favourites %d", db_favourites ? db_favourites->size() : 0);
        }
        return db_favourites.get();
    }
    if (type == Contact && !pos_contacts.all()) {
        if (db_contacts == nullptr) {
            db_contacts = DBServiceAPI::ContactGetLimitOffset(this->application, pos_contacts.actual_index, max_num);
            pos_contacts.viewed_count = 0;
            LOG_INFO("Get contacts: contacts %d : actual_index %d, max_num %d", db_contacts ? db_contacts->size() : 0, pos_contacts.actual_index,
                     pos_contacts.max_index);
        } else {
            LOG_INFO("All contacts retrieved...");
        }
        return db_contacts.get();
    }
    return nullptr;
}

bool ContactListModel::getPreviousContacts(ContactType type)
{
    auto tmp_lambda = [](Pos &pos, std::unique_ptr<std::vector<ContactRecord>> &ptr) {
        ptr = nullptr;
        if (pos.actual_index < (pos.viewed_count + pos.max_index)) {
            pos.actual_index = 0;
        } else {
            pos.actual_index -= (pos.viewed_count + pos.max_index);
        }
        pos.viewed_count = 0;
    };
    if (type == Favouirte && pos_favourites.actual_index != 0) {
        tmp_lambda(pos_favourites, db_favourites);
        return true;
    }
    if (type == Contact) {
        tmp_lambda(pos_contacts, db_contacts);
        return true;
    }
    return false;
}

bool ContactListModel::getNextContacts(ContactType type)
{
    if (type == Favouirte) {
        if (!pos_favourites.all()) {
            db_favourites = nullptr;
            pos_favourites.actual_index += pos_favourites.viewed_count;
            LOG_INFO("Get next favourites %d %d", pos_favourites.actual_index, pos_favourites.viewed_count);
            return true;
        }
    }
    if (type == Contact) {
        LOG_INFO("Get next contacts %d %d", pos_contacts.actual_index, pos_contacts.viewed_count);
        if (!pos_contacts.all()) {
            db_contacts = nullptr;
            pos_contacts.actual_index += pos_contacts.viewed_count;
            return true;
        }
    }
    return false;
}

ContactRecord ContactListModel::getContactFromPos(ContactListModel::ContactType type, uint32_t pos)
{
    if(type == Contact) {
        if(db_contacts->size() > pos) {
            return (*db_contacts)[pos];
        }
    } else if(type == Favouirte) {
        if(db_favourites->size() > pos) {
            return (*db_favourites)[pos];
        }
    }
    return ContactRecord();
}
