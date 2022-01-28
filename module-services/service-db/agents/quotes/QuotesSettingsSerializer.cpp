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
        for (const auto &[type, id] : list) {
            output.append(utils::to_string(static_cast<int>(type)) + ',');
            output.append(utils::to_string(id) + ',');
        }
        return output;
    }
    auto QuotesSettingsSerializer::deserialize(const std::string &listString) -> IdList
    {
        std::stringstream ss(listString);
        IdList list;
        QuoteID quoteID;
        bool type = true;
        for (int i; ss >> i;) {
            if (type) {
                quoteID.first = static_cast<QuoteType>(i);
                type          = false;
            }
            else {
                quoteID.second = i;
                list.push_back(quoteID);
                type = true;
            }
            if (ss.peek() == ',' || ss.peek() == ' ') {
                ss.ignore();
            }
        }
        return list;
    }
} // namespace Quotes
