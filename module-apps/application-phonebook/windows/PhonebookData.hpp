#pragma once

#include "../ContactBuilder.hpp"
#include <SwitchData.hpp>
#include <functional>
#include <Item.hpp>

namespace gui {

class PhonebookContactData : public SwitchData {
  public:
    ContactBuilder contact;
    PhonebookContactData() = default;
    PhonebookContactData(ContactBuilder contact) : contact(contact) {}
    PhonebookContactData(ContactRecord contact) : contact(contact) {}
    PhonebookContactData(PhonebookContactData *data) { this->contact = data->contact; }
    virtual ~PhonebookContactData() = default;
};

class PhonebookInfoData : public SwitchData {
    public:
        UTF8 format_str;
        std::vector<UTF8> data;
        /// text to show + callbacks to run on click
        std::vector<std::pair<UTF8, std::function<bool(Item&)>>> options;

        PhonebookInfoData() = default;
        PhonebookInfoData(PhonebookInfoData &&) = default;
        PhonebookInfoData(PhonebookInfoData * data) { this->format_str = data->format_str; this->data = data->data; this->options = data->options;}
        virtual ~PhonebookInfoData() = default;
};

}; // namespace gui
