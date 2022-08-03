// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BasePresenter.hpp>
#include <Application.hpp>
#include <SwitchData.hpp>

class SimPINSettingsWindowContract
{
  public:
    class View
    {
      public:
        virtual ~View() noexcept                       = default;
        virtual void setNavbarCenterActive(bool state) = 0;
        virtual void setTitle(const UTF8 &text)        = 0;
        virtual void refreshOptionsList()              = 0;
    };
    class Presenter : public app::BasePresenter<SimPINSettingsWindowContract::View>
    {
      public:
        virtual ~Presenter() noexcept                                        = default;
        virtual void setCurrentPinState(bool state) noexcept                 = 0;
        virtual void togglePinState()                                        = 0;
        virtual bool isPinEnabled() const noexcept                           = 0;
        virtual void requestLockState() const                                = 0;
        virtual void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) = 0;
    };
};

class SimPINSettingsPresenter : public SimPINSettingsWindowContract::Presenter
{
  public:
    explicit SimPINSettingsPresenter(app::ApplicationCommon *application);
    void setCurrentPinState(bool state) noexcept override;
    void togglePinState() override;
    bool isPinEnabled() const noexcept override;
    void requestLockState() const override;
    void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;

  private:
    app::ApplicationCommon *application = nullptr;
    bool isPinOn                        = false;
    bool pinLockStateChanged            = false;
};
