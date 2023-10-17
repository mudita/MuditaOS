// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/system/SARInfoRepository.hpp>

#include <BasePresenter.hpp>

class SARInfoWindowContract
{
  public:
    class View
    {
      public:
        virtual ~View() noexcept = default;
    };
    class Presenter : public app::BasePresenter<SARInfoWindowContract::View>
    {
      public:
        ~Presenter() noexcept override = default;

        virtual std::string getSarInfo() = 0;
    };
};

class SARInfoWindowPresenter : public SARInfoWindowContract::Presenter
{
  public:
    explicit SARInfoWindowPresenter(std::unique_ptr<AbstractSARInfoRepository> &&sarInfoRepository);

    std::string getSarInfo() override;

  private:
    std::unique_ptr<AbstractSARInfoRepository> sarInfoRepository;
};
