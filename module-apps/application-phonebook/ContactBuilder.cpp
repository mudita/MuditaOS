#include "ContactBuilder.hpp"
#include <i18/i18.hpp>

/// favourite contact marker
static const std::string favMarker = "x";

ContactBuilder::ContactBuilder() {}

ContactBuilder::ContactBuilder(std::map<std::string, UTF8> &params) { load(params); }

ContactBuilder::ContactBuilder(ContactRecord contact) { load(contact); }

void ContactBuilder::load(std::map<std::string, UTF8> &params) {
    // TODO these are right now unused!
    // utils::localize.get("app_phonebook_address")
    //  UTF8 country = ""
    //  UTF8 city;
    //  UTF8 street;
    //  UTF8 number;
    //  UTF8 assetPath;
    //  uint32_t dbID;
    //  ContactAddressType addressType;
    //  ContactType contactType;
    mapping = params;
    record.primaryName = params[utils::localize.get("app_phonebook_first_name")];
    record.alternativeName = params[utils::localize.get("app_phonebook_second_name")];
    record.note = params[utils::localize.get("app_phonebook_note")];
    record.mail = params[utils::localize.get("app_phonebook_email")];
    // TODO some sanitation should be added here
    record.numbers = {
        ContactRecord::Number(params[utils::localize.get("app_phonebook_number")], params[utils::localize.get("app_phonebook_number")]),
        ContactRecord::Number(params[utils::localize.get("app_phonebook_second_number")], params[utils::localize.get("app_phonebook_second_number")]),
    };
    record.isOnFavourites = favMarker.compare(mapping[utils::localize.get("app_phonebook_favourites")].c_str()) ==0;
    LOG_INFO("IS ON FAVOURITES: %d : %s %s", record.isOnFavourites, favMarker.c_str(), mapping[utils::localize.get("app_phonebook_favourites")].c_str());
    if(params[utils::localize.get("app_phonebook_speed_dial_key")].length() > 0) {
        record.speeddial = std::stoul(params[utils::localize.get("app_phonebook_speed_dial_key")].c_str());
    } else {
        record.speeddial = -1;
    }
}

void ContactBuilder::load(ContactRecord contact) {
    record = contact;
    mapping[utils::localize.get("app_phonebook_first_name")] = contact.primaryName;
    mapping[utils::localize.get("app_phonebook_second_name")] = contact.alternativeName;
    mapping[utils::localize.get("app_phonebook_note")] = contact.note;
    mapping[utils::localize.get("app_phonebook_email")] = contact.mail;
    // TODO it's X for now - generaly it would be nice if such symbol as tick/x was in font
    mapping[utils::localize.get("app_phonebook_favourites")] = record.isOnFavourites ? "x" : "";
    mapping[utils::localize.get("app_phonebook_speed_dial_key")] = std::to_string(contact.speeddial);
    for (unsigned int i = 0; i < record.numbers.size(); ++i) {
        if (i == 0)
            mapping[utils::localize.get("app_phonebook_number")] = record.numbers[i].numberUser;
        else if (i == 1)
            mapping[utils::localize.get("app_phonebook_second_number")] = record.numbers[i].numberUser;
        else {
            LOG_WARN("To many contact numbers to show: %d", record.numbers.size());
            break;
        }
    }
}

bool ContactBuilder::verify() { return !(record.primaryName == "" || record.numbers.size() == 0); }

ContactRecord &ContactBuilder::getRecord() { return record; }

std::map<std::string, UTF8> ContactBuilder::getMap() { return mapping; }

UTF8& ContactBuilder::speeddial_str()
{
    return mapping[utils::localize.get("app_phonebook_speed_dial_key")];
}
