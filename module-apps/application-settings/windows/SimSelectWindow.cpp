#include "SimSelectWindow.hpp"
#include "Info.hpp"
#include "SettingsMainWindow.hpp"
#include "i18/i18.hpp"
#include "log/log.hpp"
#include <bsp/cellular/bsp_cellular.hpp>
#include <common_data/EventStore.hpp>

// TODO select sim from settings in DB
std::list<gui::Option> simSelectWindow(app::Application *app)
{
    return {
        {"SIM 1",
         [=](gui::Item &item) {
             LOG_INFO("Switch SIM 1");
             Store::GSM::get()->selected = Store::GSM::SIM::SIM1;
             bsp::cellular::sim::sim_sel();
             bsp::cellular::sim::hotswap_trigger();
             return true;
         },
         gui::Arrow::Disabled},
        {"SIM 2",
         [=](gui::Item &item) {
             LOG_INFO("Switch SIM 2");
             Store::GSM::get()->selected = Store::GSM::SIM::SIM2;
             bsp::cellular::sim::sim_sel();
             bsp::cellular::sim::hotswap_trigger();
             return true;
         },
         gui::Arrow::Disabled},
    };
}
