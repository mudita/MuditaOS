// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "PlatformAgnosticSequences.hpp"

class AlarmDeactivateSequence : public AlarmChangeSequence
{
  public:
    AlarmDeactivateSequence() : AlarmChangeSequence(KeyMap::DeepPressDown)
    {}
};
