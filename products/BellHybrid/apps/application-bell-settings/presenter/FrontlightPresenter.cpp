// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrontlightPresenter.hpp"

namespace app::bell_settings
{
    FrontlightPresenter::FrontlightPresenter(std::shared_ptr<FrontlightListItemProvider> &&provider,
                                             std::unique_ptr<AbstractFrontlightModel> &&frontlightModel)
        : provider{std::move(provider)}, frontlightModel{std::move(frontlightModel)}
    {
        this->frontlightModel->onReady = [this]() {
            for (const auto &item : this->provider->getListItems()) {
                item->setValue();
            }
        };
    }

    FrontlightPresenter::~FrontlightPresenter()
    {}

    auto FrontlightPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return provider;
    }

    void FrontlightPresenter::eraseProviderData()
    {
        provider->clearData();
    }

    void FrontlightPresenter::saveConfig()
    {
        frontlightModel->saveConfig();
    }

    void FrontlightPresenter::revertConfig()
    {
        frontlightModel->revertConfig();
    }

    void FrontlightPresenter::setBacklight()
    {
        frontlightModel->setBacklight(BacklightState::On);
    }

    bool FrontlightPresenter::isConfigSaved()
    {
        return frontlightModel->isConfigSaved();
    }
} // namespace app::bell_settings
