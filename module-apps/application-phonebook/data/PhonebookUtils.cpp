#include "PhonebookUtils.hpp"

auto PhonebookUtils::formatVCard(const ContactRecord &contact) -> const std::string
{
    const std::string priNumber = (contact.numbers.size() > 0) ? contact.numbers[0].numberE164.c_str() : "";
    const std::string secNumber = (contact.numbers.size() > 1) ? contact.numbers[1].numberE164.c_str() : "";
    std::ostringstream vcard;
    vcard << "BEGIN:VCARD\n"
          << "VERSION:3.0\n"
          << "N:" << contact.alternativeName.c_str() << ";" << contact.primaryName.c_str() << ";;;\n"
          << "FN:" << contact.primaryName.c_str() << " " << contact.alternativeName.c_str() << "\n"
          << "TEL;TYPE=HOME,VOICE:" << priNumber << "\n";

    if (secNumber.length() > 0) {
        vcard << "TEL;TYPE=HOME,VOICE:" << secNumber << "\n";
    }

    vcard << "ADR;TYPE=HOME:;;" << contact.address.c_str() << "\n"
          << "EMAIL:" << contact.mail.c_str() << "\n"
          << "END:VCARD";

    LOG_INFO("formatted vcard:");
    LOG_INFO("%s", vcard.str().c_str());

    return (vcard.str());
}
