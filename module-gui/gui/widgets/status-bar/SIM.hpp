// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Image.hpp"
#include "status-bar/StatusBarWidgetBase.hpp"
#include <EventStore.hpp>

namespace gui::status_bar
{
    class SIMConfiguration : public StatusBarVisitor
    {
      public:
        enum class DisplayMode
        {
            AnyState,
            OnlyInactiveState
        };

        explicit SIMConfiguration(DisplayMode mode);

        [[nodiscard]] DisplayMode getMode() const noexcept;

        void visit([[maybe_unused]] gui::status_bar::SIM &widget) const override;

      private:
        DisplayMode mode;
    };

    class SIM : public StatusBarWidgetBase<Image>
    {
        Store::GSM::SIM current            = Store::GSM::SIM::SIM_UNKNOWN;
        SIMConfiguration::DisplayMode mode = SIMConfiguration::DisplayMode::AnyState;
        friend class SIMConfiguration;

      public:
        SIM(Item *parent, uint32_t x, uint32_t y);

        /// check if sim set in state -> if not -> show new sim
        void update();

        void show() override;

        void acceptStatusBarVisitor(StatusBarVisitor &visitor) override;
    };

    class SIMDevelopersMode : public SIM
    {
      public:
        SIMDevelopersMode(Item *parent, uint32_t x, uint32_t y);

        void show() override;
    };
} // namespace gui::status_bar
