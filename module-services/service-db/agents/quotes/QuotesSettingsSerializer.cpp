// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesSettingsSerializer.hpp"
#include <log/log.hpp>
#include <module-utils/utility/Utils.hpp>

namespace Quotes
{

    auto QuotesSettingsSerializer::serialize(const IdList &list) -> std::string
    {
        std::string output;
        for (const auto &item : list) {
            output.append(utils::to_string(item) + ',');
        }
        return output;
    }
    auto QuotesSettingsSerializer::deserialize(const std::string &listString) -> IdList
    {
        std::stringstream ss(listString);
        IdList list;
        for (int i; ss >> i;) {
            list.push_back(i);
            if (ss.peek() == ',' || ss.peek() == ' ') {
                ss.ignore();
            }
        }
        return list;
    }
} // namespace Quotes
