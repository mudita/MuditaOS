// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-bell-main/models/BellAlarmRepository.hpp"
#include "BasePresenter.hpp"

class BellAlarmWindowContract
{
  public:
    class View
    {
      public:
        virtual ~View() noexcept = default;
    };
    class Presenter : public app::BasePresenter<BellAlarmWindowContract::View>
    {
      public:
        ~Presenter() noexcept override = default;

        virtual unsigned int getHour()   = 0;
        virtual unsigned int getMinute() = 0;

        virtual BellAlarm::Status getAlarmStatus()              = 0;
        virtual utils::time::Locale::TimeFormat getTimeFormat() = 0;
    };
};

class BellAlarmPresenter : public BellAlarmWindowContract::Presenter
{
  public:
    explicit BellAlarmPresenter(std::unique_ptr<AbstractBellAlarmRepository> &&bellAlarmRepository);

    unsigned int getHour() override;
    unsigned int getMinute() override;
    BellAlarm::Status getAlarmStatus() override;
    utils::time::Locale::TimeFormat getTimeFormat() override;

  private:
    std::unique_ptr<AbstractBellAlarmRepository> bellAlarmRepository;
};
