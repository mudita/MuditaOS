#include "QuerySMSSearch.hpp"

namespace db::query
{
    SMSSearch::SMSSearch(std::string text_to_search, unsigned int starting_position, unsigned int depth)
        : text(text_to_search), starting_postion(starting_position), depth(depth)
    {}
} // namespace db::query
