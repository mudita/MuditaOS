#include "SMSTextToSearch.hpp"

SMSTextToSearch::SMSTextToSearch(const std::string text_to_search) : text_to_search(text_to_search)
{}

auto SMSTextToSearch::getTextToSearch() const -> std::string
{
    return text_to_search;
}
