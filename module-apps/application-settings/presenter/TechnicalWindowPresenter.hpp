// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-settings/models/FactoryData.hpp"
#include "BasePresenter.hpp"

class TechnicalWindowContract
{
  public:
    class View
    {
      public:
        virtual ~View() noexcept = default;
    };
    class Presenter : public app::BasePresenter<TechnicalWindowContract::View>
    {
      public:
        virtual ~Presenter() noexcept override = default;

        virtual auto getModel() -> std::string               = 0;
        virtual auto getCase() -> std::string                = 0;
        virtual auto getSerial() -> std::string              = 0;
        virtual auto getBatteryRev() -> std::string          = 0;
        virtual auto getPcb(std::string type) -> std::string = 0;
    };
};

class TechnicalWindowPresenter : public TechnicalWindowContract::Presenter
{
  public:
    static constexpr auto PCB_LM = "lm";
    static constexpr auto PCB_AM = "am";
    static constexpr auto PCB_MB = "mb";
    static constexpr auto PCB_UM = "um";

    explicit TechnicalWindowPresenter(std::unique_ptr<AbstractFactoryData> &&factoryData);

    auto getModel() -> std::string override;
    auto getCase() -> std::string override;
    auto getSerial() -> std::string override;
    auto getBatteryRev() -> std::string override;
    auto getPcb(std::string type) -> std::string override;

  private:
    std::unique_ptr<AbstractFactoryData> factoryData;
};
