// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallActionsParams.hpp"

using namespace app::manager::actions;

CallParams::CallParams(const utils::PhoneNumber::View &number) : number{number}
{}

const utils::PhoneNumber::View &CallParams::getNumber() const
{
    return number;
}
