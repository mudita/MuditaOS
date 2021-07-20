// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmPresenter.hpp"

BellAlarmPresenter::BellAlarmPresenter(std::unique_ptr<AbstractBellAlarmRepository> &&bellAlarmRepository)
    : bellAlarmRepository{std::move(bellAlarmRepository)}
{}

unsigned int BellAlarmPresenter::getHour()
{
    return bellAlarmRepository->getSystemHour();
}

unsigned int BellAlarmPresenter::getMinute()
{
    return bellAlarmRepository->getSystemMinute();
}

BellAlarm::Status BellAlarmPresenter::getAlarmStatus()
{
    return bellAlarmRepository->getSystemAlarmStatus();
}

utils::time::Locale::TimeFormat BellAlarmPresenter::getTimeFormat()
{
    return bellAlarmRepository->getSystemTimeFormat();
}
