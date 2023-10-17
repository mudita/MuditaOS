// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "VolteAllowedListInterface.hpp"
#include "ImsiParser.hpp"
#include <vector>

namespace cellular::service
{
    class VolteAllowedList : public VolteAllowedListInterface
    {
      public:
        VolteAllowedList()
        {
            buildList();
        }

        auto isVolteAllowed(const std::string &imsi) const -> bool final;
        auto getSupportStatus(const std::string &imsi) const -> ImsiParser::SupportStatus final;

      private:
        void buildList();
        std::vector<cellular::service::ImsiParser> allowedList;
    };
} // namespace cellular::service
