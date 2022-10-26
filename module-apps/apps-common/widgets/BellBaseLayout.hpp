// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ThreeBox.hpp>
#include <ImageBox.hpp>

namespace style::bell_base_layout
{
    constexpr inline auto w                   = style::window_width;
    constexpr inline auto h                   = style::window_height;
    constexpr inline auto first_layout_min_h  = 30U;
    constexpr inline auto outer_layouts_w     = 448U;
    constexpr inline auto outer_layouts_h     = 102U;
    constexpr inline auto outer_layout_margin = 38U;
    constexpr inline auto center_layout_w     = 504U;
    constexpr inline auto center_layout_h     = h - 2 * outer_layout_margin - 2 * outer_layouts_h;

    enum class ParentType
    {
        SideListView,
        Window
    };
} // namespace style::bell_base_layout

namespace gui
{
    class BellBaseLayout : public VThreeBox<VBox, VBox, VBox>
    {
      public:
        enum class Arrow
        {
            Left,
            Right
        };

        explicit BellBaseLayout(Item *parent,
                                Position x                               = 0,
                                Position y                               = 0,
                                Length w                                 = 0,
                                Length h                                 = 0,
                                bool withSideArrows                      = true,
                                style::bell_base_layout::ParentType type = style::bell_base_layout::ParentType::Window);

        [[nodiscard]] Item *getCenterBox() const noexcept;
        void resizeCenter();
        void resize();

        void setArrowVisible(Arrow arrow, bool isVisible);
        void setMinMaxArrowsVisibility(bool minCondition, bool maxCondition);

      private:
        HThreeBox<HBox, HBox, HBox> *centerThreeBox{nullptr};
        ImageBox *leftArrow{nullptr};
        ImageBox *rightArrow{nullptr};

        void addSideArrows();
    };

} /* namespace gui */
