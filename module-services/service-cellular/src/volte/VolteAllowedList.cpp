// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "VolteAllowedList.hpp"
#include "ImsiParser_Poland.hpp"
#include "ImsiParser_UnitedStates.hpp"
#include "ImsiParser_Netherlands.hpp"
#include "ImsiParser_Germany.hpp"
#include "ImsiParser_Denmark.hpp"
#include "ImsiParser_GreatBritain.hpp"
#include "ImsiParser_Canada.hpp"
#include "ImsiParser_Austria.hpp"
#include "ImsiParser_Egypt.hpp"
#include "module-utils/log/Logger.hpp"

namespace
{
    template <typename Container, typename... Items>
    void pushBack(Container &container, Items &&...items)
    {
        (container.push_back(std::forward<Items>(items)), ...);
    }
} // namespace

namespace cellular::service
{
    auto VolteAllowedList::isVolteAllowed(const std::string &imsi) const -> bool
    {
        for (const auto &country : allowedList) {
            if (country.isAllowed(imsi)) {
                LOG_INFO("MCC supported, VoLTE allowed.");
                return true;
            }
        }

        LOG_ERROR("MCC not supported, VoLTE not allowed.");
        return false;
    }

    void VolteAllowedList::buildList()
    {
        pushBack(allowedList,
                 ImsiParserPL(),
                 ImsiParserUS(),
                 ImsiParserDK(),
                 ImsiParserDE(),
                 ImsiParserNL(),
                 ImsiParserGB(),
                 ImsiParserCA(),
                 ImsiParserAT(),
                 ImsiParserEG());
    }
} // namespace cellular::service
