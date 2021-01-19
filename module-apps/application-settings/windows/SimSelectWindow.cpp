// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimSelectWindow.hpp"
#include "Info.hpp"
#include "SettingsMainWindow.hpp"
#include <i18n/i18n.hpp>
#include "log/log.hpp"
#include <common_data/EventStore.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <service-db/Settings.hpp>
#include <module-utils/Utils.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>
#include <module-apps/application-settings/ApplicationSettings.hpp>

std::list<gui::Option> simSelectWindow(app::Application *app, app::SimSetter *setter)
{
    std::list<gui::Option> l;
    l.emplace_back(gui::Option{"SIM 1",
                               [=](gui::Item &item) {
                                   setter->setSim(Store::GSM::SIM::SIM1);
                                   return true;
                               },
                               gui::option::Arrow::Disabled});
    l.emplace_back(gui::Option{"SIM 2",
                               [=](gui::Item &item) {
                                   setter->setSim(Store::GSM::SIM::SIM2);
                                   return true;
                               },
                               gui::option::Arrow::Disabled});
    return l;
}
