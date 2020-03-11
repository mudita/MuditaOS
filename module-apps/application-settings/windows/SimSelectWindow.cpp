#include "SimSelectWindow.hpp"
#include "Info.hpp"
#include "SettingsMainWindow.hpp"
#include "i18/i18.hpp"
#include "log/log.hpp"
#include <bsp/cellular/bsp_cellular.hpp>
#include <common_data/EventStore.hpp>
#include <service-db/api/DBServiceAPI.hpp>

void changeSim(app::Application *app, SettingsRecord::ActiveSim simsettings, Store::GSM::SIM sim)
{
    app->getSettings().activeSIM = SettingsRecord::ActiveSim::SIM1;
    DBServiceAPI::SettingsUpdate(app, app->getSettings());
    Store::GSM::get()->selected = Store::GSM::SIM::SIM1;
    bsp::cellular::sim::sim_sel();
    bsp::cellular::sim::hotswap_trigger();
}

std::list<gui::Option> simSelectWindow(app::Application *app)
{
    return {
        {"SIM 1",
         [=](gui::Item &item) {
             changeSim(app, SettingsRecord::ActiveSim::SIM1, Store::GSM::SIM::SIM1);
             return true;
         },
         gui::Arrow::Disabled},
        {"SIM 2",
         [=](gui::Item &item) {
             changeSim(app, SettingsRecord::ActiveSim::SIM2, Store::GSM::SIM::SIM2);
             return true;
         },
         gui::Arrow::Disabled},
    };
}
