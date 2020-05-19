#pragma once

#include <Common/Query.hpp>
#include <string>

namespace db::query
{
    /// implements search for SMS by text
    class SMSSearch : public Query
    {
      public:
        std::string text;
        unsigned int starting_postion;
        unsigned int depth;
        SMSSearch(std::string text_to_search, unsigned int starting_position, unsigned int depth);
    };
}; // namespace db::query
