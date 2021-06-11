// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SIM.hpp"

namespace gui::status_bar
{
    namespace
    {
        constexpr auto sim1       = "sim1_status"; // sim 1 indicator
        constexpr auto sim2       = "sim2_status"; // sim 2 indicator
        constexpr auto simunknown = "simunknown"; // sim - unknown sim state indicator (i.e. no initialization was done)
        constexpr auto simfailed  = "simfail";    // sim - notification for sim failure
    }                                             // namespace

    SIM::SIM(Item *parent, uint32_t x, uint32_t y) : StatusBarWidgetBase(parent, x, y, 0, 0)
    {
        set(simunknown);
    }

    void SIM::update()
    {
        using namespace cellular::api;
        auto state = currentSimState();
        auto sim   = currentSimSlot();

        if (currentState == state) {
            if (state == SimState::Ready) {
                if (currentSim == sim) {
                    return;
                }
            }
            else {
                return;
            }
        }

        currentState = state;
        currentSim   = sim;

        switch (state) {
        case SimState::Invalid:
        case SimState::Init:
            set(simunknown);
            break;
        case SimState::Fail:
            set(simfailed);
            break;
        case SimState::Ready:
            set((sim == SimSlot::SIM1) ? sim1 : sim2);
            break;
        }
    }

    void SIM::show()
    {
        bool isVisible = !(mode == SIMConfiguration::DisplayMode::OnlyInactiveState &&
                           (currentState == cellular::api::SimState::Ready));
        StatusBarWidgetBase<Image>::setVisible(isVisible);
    }

    void SIM::acceptStatusBarVisitor(StatusBarVisitor &visitor)
    {
        visitor.visit(*this);
    }

    SIMConfiguration::SIMConfiguration(DisplayMode mode) : mode{mode}
    {}

    void SIMConfiguration::visit(gui::status_bar::SIM &widget) const
    {
        widget.mode = getMode();
    }

    SIMConfiguration::DisplayMode SIMConfiguration::getMode() const noexcept
    {
        return mode;
    }

    SIMDevelopersMode::SIMDevelopersMode(Item *parent, uint32_t x, uint32_t y) : SIM(parent, x, y)
    {}

    void SIMDevelopersMode::show()
    {
        StatusBarWidgetBase<Image>::setVisible(true);
    }
}; // namespace gui::status_bar
