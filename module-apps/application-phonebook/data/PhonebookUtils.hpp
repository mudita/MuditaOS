#pragma once

#include <ContactRecord.hpp>
#include <Utils.hpp>

namespace phonebookUtils
{
    void fillContactData(std::string &data, ContactRecord &contact);
    auto formatVCard(const ContactRecord &) -> const std::string;
} // namespace phonebookUtils
