// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ThreeBox.hpp>
#include <ImageBox.hpp>

namespace style::bell_base_layout
{
    constexpr inline auto w                  = style::window_width;
    constexpr inline auto h                  = style::window_height;
    constexpr inline auto first_layout_min_h = 50U;
    constexpr inline auto outer_layouts_h    = 120U;

    namespace arrow
    {
        inline constexpr auto w = 64U;
        inline constexpr auto h = 64U;
    } // namespace arrow

    namespace centerbox
    {
        constexpr inline auto h = (style::window_height - 2 * outer_layouts_h);
    } // namespace centerbox
} // namespace style::bell_base_layout

namespace gui
{
    class BellBaseLayout : public VThreeBox<VBox, VBox, VBox>
    {
      public:
        explicit BellBaseLayout(
            Item *parent, Position x = 0, Position y = 0, Length w = 0, Length h = 0, bool withSideArrows = true);

        [[nodiscard]] Item *getCenterBox() const noexcept;

      private:
        HThreeBox<HBox, HBox, HBox> *centerThreeBox{nullptr};
        ImageBox *leftArrow{nullptr};
        ImageBox *rightArrow{nullptr};

        void addSideArrows();
    };

} /* namespace gui */
