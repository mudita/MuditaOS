// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BasePresenter.hpp>
#include <application-settings/models/network/SimContactsImportModel.hpp>

class SimContactsImportWindowContract
{
  public:
    class View
    {
      public:
        virtual ~View() noexcept = default;
    };
    class Presenter : public app::BasePresenter<SimContactsImportWindowContract::View>
    {
      public:
        virtual ~Presenter() noexcept = default;

        virtual std::shared_ptr<gui::ListItemProvider> getSimContactsProvider() const = 0;
        virtual void clearProviderData() const                                        = 0;
    };
};

class SimContactsImportWindowPresenter : public SimContactsImportWindowContract::Presenter
{
  public:
    explicit SimContactsImportWindowPresenter(std::shared_ptr<SimContactsImportModel> simContactsProvider);

    std::shared_ptr<gui::ListItemProvider> getSimContactsProvider() const override;
    void clearProviderData() const override;

  private:
    std::shared_ptr<SimContactsImportModel> simContactsProvider;
};
