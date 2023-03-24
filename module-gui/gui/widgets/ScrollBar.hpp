// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <log/log.hpp>
#include <Rect.hpp>
#include <magic_enum.hpp>
#include <type_traits>

namespace gui
{
    namespace listview
    {
        /// Possible List scrolling directions
        enum class Direction
        {
            Top,
            Bottom
        };
    }

    enum class ScrollBarType
    {
        None,         ///< None - no scroll bar for a particular control (not created at all)
        Proportional, ///< Proportional - scroll bar size calculated based on elements count in model and currently
                      ///< displayed number of elements. Use with large unequal heights lists elements.
        Fixed,        ///< Fixed - scroll bar size calculated based on fixed equal elements sizes in list.
                      ///< Use when all elements have equal heights.
        PreRendered   ///< PreRendered - scroll bar size calculated based on pre rendered pages on whole list. Use
                      ///< when elements are not equal heights but there are few of them as its renders whole
                      ///< context and can be time consuming.
    };

    struct ListViewScrollBarUpdateData
    {
        const unsigned startIndex;
        const unsigned listPageSize;
        const unsigned elementsCount;
        const unsigned elementMinimalSpaceRequired;
        const listview::Direction direction;
        const Boundaries boundaries;
    };

    template <typename OwningControl, typename UpdateData>
    struct ScrollBar: Rect
    {
        ScrollBar(Item *parent, unsigned int x, unsigned int y, unsigned int w, unsigned int h, ScrollBarType type)
            : Rect{parent, x, y, w, h}, type{type}, performUpdate_{std::bind(&ScrollBar<OwningControl, UpdateData>::genericPerformUpdate, this, std::placeholders::_1)}
        {
            setRadius(radius);
            setFillColor(color);
            activeItem = false;
        }

        void updateStartConditions(size_t storedStartIndex, size_t currentPage, size_t pagesCount)
        {
            storedStartIndex_ = storedStartIndex;
            currentPage_      = currentPage;
            pagesCount_       = pagesCount;
            if (pagesCount_ == 0) pagesCount_ = 1;
        }

        void update(UpdateData const& data)
        {
            if (!shouldShowScroll(data.listPageSize, data.elementsCount)) {
                setVisible(false);
            }

            performUpdate_(data);
            setVisible(true);
        }

        bool shouldShowScroll(size_t currentPageSize, size_t elementsCount)
        {
            return ((parent->widgetArea.w > style::listview::scroll::min_space) &&
                    (parent->widgetArea.h > style::listview::scroll::min_space) && (currentPageSize < elementsCount));
        }

        void setTopMargin(int topMargin)
        {
            topMargin_ = topMargin;
        }

        ScrollBarType type;

      protected:
        size_t storedStartIndex_ = 0;
        size_t currentPage_      = std::numeric_limits<size_t>::max();
        size_t pagesCount_       = 1;
        int topMargin_           = style::margins::big;

        static inline constexpr auto radius      = 2U;
        static inline constexpr auto color       = gui::Color{0, 0};
        static inline constexpr auto margin      = 5U;

        std::function<void(UpdateData const&)> performUpdate_;

        void updateProportional(UpdateData const& data)
        {
            auto const topMargin = style::margins::big;
            auto const scrollStep = (parent->widgetArea.h - topMargin) / static_cast<double>(data.elementsCount);
            auto const scrollH = scrollStep * data.listPageSize;
            auto const scrollY = (scrollStep * data.startIndex > 0) ? scrollStep * data.startIndex : topMargin;

            setArea(BoundingBox(
                parent->widgetArea.w - style::listview::scroll::margin, scrollY, style::listview::scroll::w, scrollH));
        }

        void updateFixed(UpdateData const& data)
        {
            const auto elementsOnPage = parent->widgetArea.h / data.elementMinimalSpaceRequired;

            pagesCount_ = (data.elementsCount % elementsOnPage == 0) ? data.elementsCount / elementsOnPage
                                                                     : data.elementsCount / elementsOnPage + 1;
            if (pagesCount_ == 0) pagesCount_ = 1;

            currentPage_ = data.startIndex / elementsOnPage;

            const auto scrollH = (parent->widgetArea.h - topMargin_) / pagesCount_;
            const auto scrollY = (scrollH * currentPage_ > 0) ? scrollH * currentPage_ : topMargin_;

            setArea(BoundingBox(
                parent->widgetArea.w - style::listview::scroll::margin, scrollY, style::listview::scroll::w, scrollH));
        }

        void genericPerformUpdate(UpdateData const& data)
        {
            switch (type) {
            case ScrollBarType::Proportional:
                updateProportional(data);
                break;
            case ScrollBarType::Fixed:
                updateFixed(data);
                break;
            default:
                LOG_ERROR("unsupported scroll bar type = %s for this widget", magic_enum::enum_name(type).data());
                break;
            }
        }
    };

    class ListView;

    struct ListScrollBar: ScrollBar<ListView, ListViewScrollBarUpdateData>
    {
      protected:
        using BaseType = ScrollBar<ListView, ListViewScrollBarUpdateData>;

        void updatePreRendered(ListViewScrollBarUpdateData const& data)
        {
            if (data.startIndex != this->storedStartIndex_) {
                if (data.direction == listview::Direction::Bottom) {
                    if (data.boundaries == Boundaries::Continuous && (data.startIndex == 0)) {
                        this->currentPage_ = 0;
                    }
                    else if (this->currentPage_ + 1 < this->pagesCount_) {
                        this->currentPage_++;
                    }
                }
                else {
                    if (data.boundaries == Boundaries::Continuous && this->storedStartIndex_ == 0) {
                        this->currentPage_ = this->pagesCount_ - 1;
                    }
                    else if (this->currentPage_ > 0 && this->storedStartIndex_ != 0) {
                        this->currentPage_--;
                    }
                }
            }

            this->storedStartIndex_ = data.startIndex;

            auto scrollH = this->parent->widgetArea.h / this->pagesCount_;
            auto scrollY = (this->currentPage_ * scrollH > 0) ? this->currentPage_ * scrollH : this->topMargin_;

            this->setArea(BoundingBox(
                this->parent->widgetArea.w - style::listview::scroll::margin, scrollY, style::listview::scroll::w, scrollH));
        }

      public:
        ListScrollBar(Item *parent, unsigned int x, unsigned int y, unsigned int w, unsigned int h, ScrollBarType type):
              BaseType(parent, x, y, w, h, type)
        {
            performUpdate_ = [this, upperPerformUpdate_ = performUpdate_](ListViewScrollBarUpdateData const& data) {
                if (this->type == ScrollBarType::PreRendered) {
                    return updatePreRendered(data);
                }

                return upperPerformUpdate_(data);
            };
        }
    };
}
