// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MovedSequence.hpp"

class AlarmDeactivateSequence : public MovedSequence
{
  public:
    AlarmDeactivateSequence() : MovedSequence(KeyMap::DeepPressDown)
    {}
};
