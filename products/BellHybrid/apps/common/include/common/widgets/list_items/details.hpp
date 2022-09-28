// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/AbstractSettingsModel.hpp>
#include <common/widgets/BellSideListItemWithCallbacks.hpp>
#include <common/data/StyleCommon.hpp>

#include <apps-common/widgets/spinners/Spinners.hpp>
#include <apps-common/widgets/TimeSetFmtSpinner.hpp>

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
            virtual void control_bottom_description(const value_type &value){};

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

                inputCallback = [this, &bottomDescription](Item &, const gui::InputEvent &event) {
                    return OnInputCallback(event);
                };

                focusChangedCallback = [this, &bottomDescription](Item &) {
                    OnFocusChangedCallback();
                    control_visibility();
                    return true;
                };
            }

          private:
            void control_visibility()
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
    } // namespace details

} // namespace app::list_items
