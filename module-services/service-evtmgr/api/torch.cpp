// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-evtmgr/torch.hpp"
#include "bsp/torch/torch.hpp"

// Static API for torch

namespace event::service::api
{
    bool isTorchOn()
    {
        auto [works, isOn] = bsp::torch::getState();
        assert(works);
        return isOn == bsp::torch::State::on;
    }
} // namespace event::service::api
