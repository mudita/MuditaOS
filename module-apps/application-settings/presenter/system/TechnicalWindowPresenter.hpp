// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/system/TechnicalInformationModel.hpp>

#include <BasePresenter.hpp>
#include <ListItemProvider.hpp>

class TechnicalWindowContract
{
  public:
    class View
    {
      public:
        virtual ~View() noexcept          = default;
        virtual void imeiReady() noexcept = 0;
    };
    class Presenter : public app::BasePresenter<TechnicalWindowContract::View>
    {
      public:
        virtual ~Presenter() noexcept = default;

        virtual std::shared_ptr<gui::ListItemProvider> getTechnicalInformationProvider() const = 0;
        virtual void requestImei()                                                             = 0;
    };
};

class TechnicalWindowPresenter : public TechnicalWindowContract::Presenter
{
  public:
    explicit TechnicalWindowPresenter(app::ApplicationCommon *application,
                                      std::shared_ptr<TechnicalInformationModel> technicalInformationProvider);

    std::shared_ptr<gui::ListItemProvider> getTechnicalInformationProvider() const override;
    void requestImei() override;

  private:
    app::ApplicationCommon *application{};
    std::shared_ptr<TechnicalInformationModel> technicalInformationProvider;
    std::function<void()> onImeiReady = nullptr;
};
