// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
} // namespace phonebookUtils
