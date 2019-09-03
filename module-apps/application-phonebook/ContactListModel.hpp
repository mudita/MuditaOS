#pragma once

#include <stdint.h>
#include "service-db/api/DBServiceAPI.hpp"
#include <Application.hpp>
#include <log/log.hpp>

class ContactListModel {
    struct Pos {
        uint32_t actual_index =0;   /// actual index loaded from DB != index in window
        uint32_t viewed_count =0;   /// num of actual showed contacts <= loaded contacts
        uint32_t max_index    =0;   /// maximum index which could be loaded == db size
        bool all() {
            if( actual_index ==0 && max_index == 0 ) {
                return false;
            }
            return actual_index + viewed_count == max_index;
        }
    }pos_favourites, pos_contacts;
    app::Application* application = nullptr;
    std::unique_ptr<std::vector<ContactRecord>> db_favourites=nullptr, db_contacts=nullptr;

    /// update maximum num of contacts available in DB
    bool updateMax();

    public:
        enum ContactType {
            Favouirte,
            Contact,
        };

        ContactListModel(app::Application *app=nullptr, uint32_t max_visible_el=0);

        /// check if all contacts were fetched?
        bool all();

        void updateLastCount(ContactType type, uint32_t val);

        /// there will be less contacts shown than loaded from db (due to passive elements)
        /// viewed_count has to be adjusted acordingly
        void updateLastCount(uint32_t favourite_contacts_shown_cnt, uint32_t contacts_shown_cnt);

        /// get contacts from DB, could add prefetching too, now dumb
        std::vector<ContactRecord> *getContacts(ContactType type, uint32_t max_num);

        /// doestn use base - just refresh positions
        bool getPreviousContacts(ContactType type);

        bool getNextContacts(ContactType type);

        /// return copy of contact on index pos, if none -> return empty ContactRecord
        ContactRecord getContactFromPos(ContactListModel::ContactType type, uint32_t pos);
};
