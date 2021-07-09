// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TechnicalWindowPresenter.hpp"

TechnicalWindowPresenter::TechnicalWindowPresenter(std::unique_ptr<AbstractFactoryData> &&factoryData)
    : factoryData{std::move(factoryData)}
{}

auto TechnicalWindowPresenter::getModel() -> std::string
{
    return factoryData->getModel();
}
auto TechnicalWindowPresenter::getCase() -> std::string
{
    return factoryData->getCase();
}
auto TechnicalWindowPresenter::getSerial() -> std::string
{
    return factoryData->getSerial();
}
auto TechnicalWindowPresenter::getBatteryRev() -> std::string
{
    return factoryData->getBatteryRev();
}
auto TechnicalWindowPresenter::getPcb(std::string type) -> std::string
{
    return factoryData->getPcb(type);
}