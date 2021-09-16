// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>
#include <application-settings/presenter/network/SimContactsImportWindowPresenter.hpp>
#include <application-settings/models/network/SimContactsImportModel.hpp>
#include <Icon.hpp>

namespace gui
{
    class SimContactsImportWindow : public AppWindow, public SimContactsImportWindowContract::View
    {
      public:
        SimContactsImportWindow(app::ApplicationCommon *app,
                                std::unique_ptr<SimContactsImportWindowContract::Presenter> presenter);

      private:
        ListView *list                             = nullptr;
        Icon *emptyListIcon                        = nullptr;
        std::function<void()> onEnterInputCallback = nullptr;
        std::function<void()> onLFInputCallback    = nullptr;

        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        bool onInput(const InputEvent &inputEvent) override;

        void displayDuplicatesCount(unsigned int duplicatesCount) noexcept override;
        void displayDuplicates() noexcept override;
        void contactsImported() noexcept override;
        void contactsReady() noexcept override;
        void displayProgressInfo();

        std::shared_ptr<SimContactsImportWindowPresenter::Presenter> presenter;
    };

} // namespace gui
