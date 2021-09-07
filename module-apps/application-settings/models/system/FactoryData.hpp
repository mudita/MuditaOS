// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/Settings.hpp>

class AbstractFactoryData
{
  public:
    virtual ~AbstractFactoryData() noexcept = default;

    virtual auto getModel() -> std::string               = 0;
    virtual auto getCase() -> std::string                = 0;
    virtual auto getSerial() -> std::string              = 0;
    virtual auto getBatteryRev() -> std::string          = 0;
    virtual auto getPcb(std::string type) -> std::string = 0;
};

class FactoryData : public AbstractFactoryData
{
  public:
    explicit FactoryData(settings::Settings &settings);

    auto getModel() -> std::string override;
    auto getCase() -> std::string override;
    auto getSerial() -> std::string override;
    auto getBatteryRev() -> std::string override;
    auto getPcb(std::string type) -> std::string override;

  private:
    settings::Settings &settings;
};
