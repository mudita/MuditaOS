// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <boost/sml.hpp>
#include <log.hpp>

struct Logger
{
    template <class SM, class TEvent> void log_process_event(const TEvent &)
    {
        LOG_DEBUG(
            "[%s][process_event] %s\n", boost::sml::aux::get_type_name<SM>(), boost::sml::aux::get_type_name<TEvent>());
    }

    template <class SM, class TGuard, class TEvent> void log_guard(const TGuard &, const TEvent &, bool result)
    {
        LOG_DEBUG("[%s][guard] %s %s %s\n",
                  boost::sml::aux::get_type_name<SM>(),
                  boost::sml::aux::get_type_name<TGuard>(),
                  boost::sml::aux::get_type_name<TEvent>(),
                  (result ? "[OK]" : "[Reject]"));
    }

    template <class SM, class TAction, class TEvent> void log_action(const TAction &, const TEvent &)
    {
        LOG_DEBUG("[%s][action] %s %s\n",
                  boost::sml::aux::get_type_name<SM>(),
                  boost::sml::aux::get_type_name<TAction>(),
                  boost::sml::aux::get_type_name<TEvent>());
    }

    template <class SM, class TSrcState, class TDstState>
    void log_state_change(const TSrcState &src, const TDstState &dst)
    {
        LOG_DEBUG("[%s][transition] %s -> %s\n", boost::sml::aux::get_type_name<SM>(), src.c_str(), dst.c_str());
    }
};
