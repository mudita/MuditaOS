// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ImmediateUpscale.hpp"

namespace sys::cpu
{
    AlgorithmResult ImmediateUpscale::calculateImplementation(const AlgorithmData &data)
    {
        const auto now = data.sentinel.frequency;
        const auto was = data.curentFrequency;
        if (now > was) {
            return {algorithm::Change::UpScaled, now};
        }
        else if (now == was) {
            return {algorithm::Change::Hold, now};
        }

        return {algorithm::Change::NoChange, was};
    }
} // namespace sys::cpu
