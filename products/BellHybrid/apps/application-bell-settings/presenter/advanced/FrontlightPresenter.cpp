// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

    auto FrontlightPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return provider;
    }

    void FrontlightPresenter::eraseProviderData()
    {
        provider->clearData();
    }
} // namespace app::bell_settings
