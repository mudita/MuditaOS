#pragma once

#include <SwitchData.hpp>

class SMSTextToSearch : public gui::SwitchData
{
    std::string text_to_search;

  public:
    SMSTextToSearch(const std::string text_to_search);
    [[nodiscard]] auto getTextToSearch() const -> std::string;
};
