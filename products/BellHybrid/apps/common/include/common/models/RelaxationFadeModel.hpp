// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "AbstractRelaxationFadeModel.hpp"
#include "SettingsModel.hpp"
#include <ApplicationCommon.hpp>

namespace app
{
    class RelaxationFadeSettings : public gui::SettingsModel<bool>
    {
      public:
        using SettingsModel::SettingsModel;

        auto setValue(bool value) -> void override;
        auto getValue() const -> bool override;
    };

    class RelaxationFadeModel : public AbstractRelaxationFadeModel
    {
      public:
        RelaxationFadeModel() = delete;

        explicit RelaxationFadeModel(ApplicationCommon *app) : fade{std::make_unique<RelaxationFadeSettings>(app)}
        {}

        auto getFade() -> gui::AbstractSettingsModel<bool> & override
        {
            return *fade;
        }

      private:
        std::unique_ptr<gui::AbstractSettingsModel<bool>> fade;
    };
} // namespace app
