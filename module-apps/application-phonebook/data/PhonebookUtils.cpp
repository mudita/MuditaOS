#include "PhonebookUtils.hpp"

namespace phonebookUtils
{
    void fillContactData(std::string &data, ContactRecord &contact)
    {
        utils::findAndReplaceAll(data, "$CONTACT_PRIMARY_NAME$", contact.primaryName);
        utils::findAndReplaceAll(data, "$CONTACT_ALTERNATIVE_NAME$", contact.alternativeName);
        utils::findAndReplaceAll(
            data, "$CONTACT_NAME$", contact.getFormattedName(ContactRecord::NameFormatType::Title));
        utils::findAndReplaceAll(
            data, "$CONTACT_NUMBER1$", (contact.numbers.size() == 1) ? contact.numbers[0].number.getEntered() : "");
        utils::findAndReplaceAll(
            data, "$CONTACT_NUMBER2$", (contact.numbers.size() == 2) ? contact.numbers[1].number.getEntered() : "");
        utils::findAndReplaceAll(data, "$CONTACT_SPEED_DIAL$", contact.speeddial);
    }

    auto formatVCard(const ContactRecord &contact) -> const UTF8
    {
        const std::string priNumber = (contact.numbers.size() > 0) ? contact.numbers[0].number.getE164().c_str() : "";
        const std::string secNumber = (contact.numbers.size() > 1) ? contact.numbers[1].number.getE164().c_str() : "";
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
} // namespace phonebookUtils
