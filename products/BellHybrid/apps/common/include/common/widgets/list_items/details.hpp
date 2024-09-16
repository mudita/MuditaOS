// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/models/AbstractSettingsModel.hpp>
#include <common/widgets/BellSideListItemWithCallbacks.hpp>
#include <common/data/StyleCommon.hpp>

#include <apps-common/widgets/spinners/Spinners.hpp>
#include <apps-common/widgets/BarGraph.hpp>
#include "common/data/ListItemBarStyle.hpp"

namespace app::list_items
{
    namespace details
    {
        template <typename SpinnerType>
        class ListItemBase : public gui::BellSideListItemWithCallbacks
        {

          public:
            using spinner_type = SpinnerType;
            using value_type   = typename SpinnerType::value_type;

            value_type value() const
            {
                return spinner->value();
            }

            void set_value(const value_type &value)
            {
                spinner->set_value(value);
                control_visibility();
            }

            void set_range(const typename spinner_type::range &range)
            {
                spinner->set_range(range);
            }

            void set_on_value_change_cb(std::function<void(const value_type &)> &&cb)
            {
                onValueChangeCb = std::move(cb);
            }

          protected:
            SpinnerType *raw_spinner()
            {
                return spinner;
            }

            /// Might be overridden if the specific handling of the bottom text is needed
            virtual void control_bottom_description(const value_type &value)
            {}

            explicit ListItemBase(typename SpinnerType::range &&range,
                                  gui::AbstractSettingsModel<value_type> &model,
                                  const std::string &topDescription    = "",
                                  const std::string &bottomDescription = "")
                : BellSideListItemWithCallbacks(topDescription), model{model}, bottomDescription{bottomDescription}
            {
                spinner = new SpinnerType(std::move(range), gui::Boundaries::Fixed);
                spinner->setMaximumSize(::style::bell_base_layout::w, ::style::bell_base_layout::h);
                spinner->setFont(gui::bell_style::font);
                spinner->setAlignment(
                    gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
                spinner->setFocusEdges(gui::RectangleEdge::None);
                body->getCenterBox()->addWidget(spinner);

                if (not this->bottomDescription.empty()) {
                    setupBottomTextBox(this->bottomDescription);
                }

                spinner->onValueChanged = [this](const auto &val) {
                    control_visibility();
                    if (onValueChangeCb) {
                        onValueChangeCb(val);
                    }
                };

                getValue = [this]() { this->model.setValue(this->spinner->value()); };
                setValue = [this]() {
                    this->spinner->set_value(this->model.getValue());
                    control_visibility();
                };

                inputCallback = [this]([[maybe_unused]] Item &item, const gui::InputEvent &event) {
                    return OnInputCallback(event);
                };

                focusChangedCallback = [this]([[maybe_unused]] Item &item) {
                    OnFocusChangedCallback();
                    control_visibility();
                    return true;
                };
            }

            virtual void control_visibility()
            {
                body->setMinMaxArrowsVisibility(spinner->is_min(), spinner->is_max());
                if (not this->bottomDescription.empty()) {
                    control_bottom_description(spinner->value());
                    body->resize();
                }
            }

            SpinnerType *spinner{};
            gui::AbstractSettingsModel<value_type> &model;
            std::string bottomDescription;
            std::function<void(const value_type &)> onValueChangeCb;
        };

        template <typename SpinnerType>
        class ListItemBaseWithBar : public ListItemBase<SpinnerType>
        {
          public:
            using spinner_type = SpinnerType;
            using value_type   = typename SpinnerType::value_type;

          protected:
            explicit ListItemBaseWithBar(typename SpinnerType::range &&range,
                                         gui::AbstractSettingsModel<value_type> &model,
                                         const std::uint8_t barSteps,
                                         const std::string &topDescription    = "",
                                         const std::string &bottomDescription = "")
                : ListItemBase<SpinnerType>(std::move(range), model, topDescription, bottomDescription)
            {
                using namespace listItemBarStyle;
                const auto progressArcRadius = progress::radius;
                const auto progressArcWidth  = progress::penWidth;
                const auto arcStartAngle     = -90 - progress::verticalDeviationDegrees;
                const auto arcSweepAngle     = 360 - (2 * (progress::verticalDeviationDegrees));

                gui::Arc::ShapeParams arcParams;
                arcParams.setCenterPoint(gui::Point(window::windowWidth / 2, window::windowHeight / 2))
                    .setRadius(progressArcRadius)
                    .setStartAngle(arcStartAngle)
                    .setSweepAngle(arcSweepAngle)
                    .setPenWidth(progressArcWidth)
                    .setBorderColor(gui::ColorFullBlack);

                progress =
                    new gui::ArcProgressBar(this, arcParams, gui::ArcProgressBar::ProgressDirection::CounterClockwise);
                progress->setMaximum(barSteps);
                progress->setValue(static_cast<unsigned int>(this->spinner->value()));
            }

            gui::ArcProgressBar *progress = nullptr;
            void control_visibility() override
            {
                ListItemBase<SpinnerType>::control_visibility();
                if (progress && this->spinner) {
                    progress->setValue(static_cast<unsigned int>(this->spinner->value()));
                }
            }
        };
    } // namespace details
} // namespace app::list_items
