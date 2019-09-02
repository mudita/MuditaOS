#pragma once

#include <i18/i18.hpp>
#include "../../ContactBuilder.hpp"

#include <log/log.hpp>

// proper contact - John Doe
ContactBuilder add_dummy_contact()
{
    std::map<std::string, UTF8> params {
        { utils::localize.get("app_phonebook_favourites"), "x", },
        { utils::localize.get("app_phonebook_first_name"), "John" },
        { utils::localize.get("app_phonebook_second_name"), "Doe" },
        { utils::localize.get("app_phonebook_number"), "600 000 000" },
        { utils::localize.get("app_phonebook_second_number"), "+48 700 000 000" },
        { utils::localize.get("app_phonebook_email"), "a@gmail.com" },
        { utils::localize.get("app_phonebook_speed_dial_key"), "2"},
        { utils::localize.get("app_phonebook_address"), "Very long street name", },
        { utils::localize.get("app_phonebook_note"), "There is nothing to note" },
        };

    ContactBuilder contact( params );
    LOG_INFO("-> %s %s",
            contact.getRecord().primaryName.c_str(),
            params[utils::localize.get("app_phonebook_favourites")].c_str());
    return contact;
}

ContactBuilder add_dummy_next(uint32_t val)
{
    std::map<std::string, UTF8> params {
        { utils::localize.get("app_phonebook_favourites"), "", },
        { utils::localize.get("app_phonebook_first_name"), "John"+std::to_string(val) },
        { utils::localize.get("app_phonebook_second_name"), "Doe" },
        { utils::localize.get("app_phonebook_number"), "600 000 01" + std::to_string(val)},
        { utils::localize.get("app_phonebook_second_number"), "+48 700 000 00" + std::to_string(val) },
        { utils::localize.get("app_phonebook_email"), "a@gmail.com" },
        { utils::localize.get("app_phonebook_speed_dial_key"), ""},
        { utils::localize.get("app_phonebook_address"), "Very long street name", },
        { utils::localize.get("app_phonebook_note"), "There is nothing to note" },
        };

    ContactBuilder contact( params );
    LOG_INFO("-> %s %s",
            contact.getRecord().primaryName.c_str(),
            params[utils::localize.get("app_phonebook_favourites")].c_str());
    return contact;
}

ContactBuilder add_dummy2_contact()
{
    std::map<std::string, UTF8> params {
        { utils::localize.get("app_phonebook_first_name"), "Jehny" },
        { utils::localize.get("app_phonebook_second_name"), "Dim" },
        { utils::localize.get("app_phonebook_number"), "100 000 000" },
        { utils::localize.get("app_phonebook_second_number"), "+48 100 000 000" },
        { utils::localize.get("app_phonebook_email"), "a@gmail.com" },
        { utils::localize.get("app_phonebook_speed_dial_key"), "3"},
        { utils::localize.get("app_phonebook_favourites"), "", },
        { utils::localize.get("app_phonebook_address"), "Very long street name", },
        { utils::localize.get("app_phonebook_note"), "There is nothing to note" },
        };
    ContactBuilder contact( params );
    LOG_INFO("-> %s %s",
            contact.getRecord().primaryName.c_str(),
            params[utils::localize.get("app_phonebook_favourites")].c_str());
    return contact;
}

// empty data
ContactBuilder add_bad_contact()
{
    return ContactBuilder();
}

// same name as  John Doe
ContactBuilder add_duplicate_contact_name()
{
    std::map<std::string, UTF8> params {
        { utils::localize.get("app_phonebook_first_name"), "John" },
        { utils::localize.get("app_phonebook_second_name"), "Doe" },
        { utils::localize.get("app_phonebook_number"), "1" },
        { utils::localize.get("app_phonebook_second_number"), "1" },
        };
    ContactBuilder contact( params );
    return contact;
}

// same number as  John Doe
ContactBuilder add_duplicate_contact_number()
{
    std::map<std::string, UTF8> params {
        { utils::localize.get("app_phonebook_first_name"), "Johny" },
        { utils::localize.get("app_phonebook_second_name"), "Bravo" },
        { utils::localize.get("app_phonebook_number"), "600 000 000" },
        { utils::localize.get("app_phonebook_second_number"), "600 000 000" },
        };
    ContactBuilder contact( params );
    return contact;
}

// same speed daial as John Doe
ContactBuilder add_duplicate_speed_dial()
{
    std::map<std::string, UTF8> params {
        { utils::localize.get("app_phonebook_first_name"), "Joanna" },
        { utils::localize.get("app_phonebook_second_name"), "Krupa" },
        { utils::localize.get("app_phonebook_number"), "100" },
        { utils::localize.get("app_phonebook_speed_dial_key"), "2"},
        };
    ContactBuilder contact( params );
    return contact;
}
