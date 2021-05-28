// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TorchHandler.hpp"
#include <bsp/torch/torch.hpp>
#include <stdexcept>

namespace torch
{
    using namespace bsp::torch;

    unsigned short TorchHandler::intensityIn_mA(Intensity val)
    {
        return max_current_mA * (val > 1 ? val = 1 : val);
    }

    void TorchHandler::init(TorchConfig config)
    {
        this->config = config;
    }

    void TorchHandler::setIntensityByColor()
    {
        auto color = getColorTemp();
        if (color == ColourTemperature::warmest) {
            setCurrent(intensityIn_mA(config.inWhite));
        }
        else {
            setCurrent(intensityIn_mA(config.inWhite));
        }
    }

    void TorchHandler::toggleOnOff()
    {
        auto [success, state] = getState();
        if (!success)
            std::runtime_error("torch get state fail");
        auto newState = (state == State::off) ? State::on : State::off;
        setIntensityByColor();
        turn(newState, ColourTemperature::coldest);
    }

    void TorchHandler::toggleColor()
    {
        auto [success, state] = getState();
        if (!success)
            std::runtime_error("torch get state fail");
        if (state == State::on) {
            auto color = getColorTemp();
            auto newColor =
                (color == ColourTemperature::coldest) ? ColourTemperature::warmest : ColourTemperature::coldest;
            setIntensityByColor();
            turn(State::on, newColor);
        }
    }

    void TorchHandler::off()
    {
        turn(State::off);
    }
} // namespace torch
