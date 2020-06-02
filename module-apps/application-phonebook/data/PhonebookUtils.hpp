#pragma once

#include <ContactRecord.hpp>

class PhonebookUtils
{
  public:
    static auto formatVCard(const ContactRecord &) -> const std::string;
};
