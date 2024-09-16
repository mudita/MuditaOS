// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#if defined(PLATFORM_linux)

#include "ReleaseSequence.hpp"
using AlarmChangeSequence = ReleaseSequence;

#elif defined(PLATFORM_rt1051)

#include "MovedSequence.hpp"
using AlarmChangeSequence = MovedSequence;

#endif
