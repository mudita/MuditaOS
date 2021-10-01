// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ReleaseSequence.hpp"

class AlarmDeactivateSequence : public ReleaseSequence
{
  public:
    AlarmDeactivateSequence() : ReleaseSequence(KeyMap::DeepPressDown)
    {}
};
