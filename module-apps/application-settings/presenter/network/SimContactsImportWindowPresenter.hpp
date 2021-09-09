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
        virtual ~View() noexcept                                                   = default;
        virtual void displayDuplicatesCount(unsigned int duplicatesCount) noexcept = 0;
        virtual void displayDuplicates() noexcept                                  = 0;
        virtual void contactsImported() noexcept                                   = 0;
        virtual void contactsReady() noexcept                                      = 0;
    };
    class Presenter : public app::BasePresenter<SimContactsImportWindowContract::View>
    {
      public:
        virtual ~Presenter() noexcept = default;

        virtual std::shared_ptr<gui::ListItemProvider> getSimContactsProvider() const = 0;
        virtual void eraseProviderData() const                                        = 0;
        virtual void saveImportedContacts()                                           = 0;
        virtual void requestDuplicates()                                              = 0;
        virtual void requestSimContacts()                                             = 0;
        virtual bool isRequestCompleted()                                             = 0;
    };
};

class SimContactsImportWindowPresenter : public SimContactsImportWindowContract::Presenter
{
  public:
    explicit SimContactsImportWindowPresenter(app::ApplicationCommon *application,
                                              std::shared_ptr<SimContactsImportModel> simContactsProvider);

    std::shared_ptr<gui::ListItemProvider> getSimContactsProvider() const override;
    void eraseProviderData() const override;
    void saveImportedContacts() override;
    void requestDuplicates() override;
    void requestSimContacts() override;
    bool isRequestCompleted() override;

  private:
    app::ApplicationCommon *application         = nullptr;
    bool requestCompleted                       = true;
    bool duplicatesChecked                      = false;
    std::function<void()> onSave                = nullptr;
    std::function<void(bool)> onDuplicatesCheck = nullptr;
    std::function<void()> onSimContactsReady    = nullptr;
    std::shared_ptr<SimContactsImportModel> simContactsProvider;
};
