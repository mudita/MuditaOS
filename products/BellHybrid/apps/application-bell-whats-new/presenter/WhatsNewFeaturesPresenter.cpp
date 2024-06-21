// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WhatsNewFeaturesPresenter.hpp"
#include "WhatsNewFeaturesLayout.hpp"
#include <product/version.hpp>

namespace app::whatsnew
{
    WhatsNewFeaturesPresenter::WhatsNewFeaturesPresenter(std::unique_ptr<models::WhatsNewFeaturesModel> &&model)
        : model{std::move(model)}
    {
        createLayouts();
    }

    auto WhatsNewFeaturesPresenter::getLayouts() const -> std::vector<gui::Item *>
    {
        return layouts;
    }

    auto WhatsNewFeaturesPresenter::isLastLayout(const gui::Item *layout) const -> bool
    {
        return !layouts.empty() && (layouts.back() == layout);
    }

    auto WhatsNewFeaturesPresenter::getFirstLayout() const -> gui::Item *
    {
        return layouts.empty() ? nullptr : layouts.front();
    }

    auto WhatsNewFeaturesPresenter::createLayouts() -> void
    {
        const auto &features = model->getFeatures();
        layouts.reserve(features.size());

        for (auto it = features.begin(); it != features.end(); ++it) {
            const auto isFirst = (it == features.begin());
            const auto isLast  = (it == std::prev(features.end()));

            auto layout = new gui::WhatsNewFeaturesLayout(it->title, it->description, it->iconName, !isFirst, !isLast);
            layouts.emplace_back(layout);
        }
    }

    auto WhatsNewFeaturesPresenter::setCurrentOsVersion() -> void
    {
        model->setCurrentOsVersion(VERSION);
    }
} // namespace app::whatsnew
