// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <list>
#include "Rect.hpp"
#include "BoxLayout.hpp"
#include "Span.hpp"
#include "ListItemProvider.hpp"
#include "Style.hpp"

namespace gui
{
    namespace listview
    {
        inline constexpr auto nPos = std::numeric_limits<unsigned int>::max();

        /// Possible List boundaries handling types
        enum class Boundaries
        {
            Fixed,     ///< Fixed - list will stop scrolling on first or last elements on appropriate top or bottom
                       ///< directions.
            Continuous ///< Continuous - list will continue to beginning or end on first or last elements on
                       ///< appropriate top or bottom directions.
        };

        /// Possible List scrolling directions
        enum class Direction
        {
            Top,
            Bottom
        };

        /// Possible List rebuild types
        enum class RebuildType
        {
            Full,          ///< Full rebuild - resets lists to all initial conditions and request data from beginning.
            InPlace,       ///< InPlace rebuild - stores currently focused part of list and rebuild from that part.
            OnPageElement, ///< OnPageElement rebuild - same page but focus changed on provided element index.
            OnOffset       ///< OnOffset rebuild - resets lists to all initial conditions and request data from provided
                           ///< offset.
        };

        /// Possible List ScrollBar types
        enum class ScrollBarType
        {
            None,         ///< None - list without scroll bar (but with scrolling).
            Proportional, ///< Proportional - scroll bar size calculated based on elements count in model and currently
                          ///< displayed number of elements. Use with large unequal heights lists elements.
            Fixed,        ///< Fixed - scroll bar size calculated based on fixed equal elements sizes in list.
                          ///< Use when all elements have equal heights.
            PreRendered   ///< PreRendered - scroll bar size calculated based on pre rendered pages on whole list. Use
                          ///< when elements are not equal heights but there are few of them as its renders whole
                          ///< context and can be time consuming.
        };

        enum class Orientation
        {
            TopBottom,
            BottomTop
        };
    } // namespace listview

    class ListItemProvider;

    using rebuildRequest = std::pair<listview::RebuildType, unsigned int>;

    struct ListViewScrollUpdateData
    {
        const unsigned int startIndex;
        const unsigned int listPageSize;
        const unsigned int elementsCount;
        const unsigned int elementMinimalHeight;
        const listview::Direction direction;
        const listview::Boundaries boundaries;
        const int topMargin;
    };

    class ListViewScroll : public Rect
    {
      private:
        unsigned int storedStartIndex = 0;
        unsigned int currentPage      = listview::nPos;
        unsigned int pagesCount       = 0;

        void updateProportional(const ListViewScrollUpdateData &data);
        void updateFixed(const ListViewScrollUpdateData &data);
        void updatePreRendered(const ListViewScrollUpdateData &data);

      public:
        listview::ScrollBarType type = listview::ScrollBarType::None;

        ListViewScroll(
            Item *parent, unsigned int x, unsigned int y, unsigned int w, unsigned int h, listview::ScrollBarType type);

        bool shouldShowScroll(unsigned int listPageSize, unsigned int elementsCount);
        void updateStartConditions(const unsigned int storedStartIndex,
                                   const unsigned int currentPage,
                                   const unsigned int pagesCount);
        void update(const ListViewScrollUpdateData &data);
    };

    class ListView : public Rect
    {
      protected:
        unsigned int startIndex                    = 0;
        unsigned int storedFocusIndex              = listview::nPos;
        unsigned int elementsCount                 = 0;
        std::shared_ptr<ListItemProvider> provider = nullptr;
        VBox *body                                 = nullptr;
        ListViewScroll *scroll                     = nullptr;
        std::list<rebuildRequest> rebuildRequests;
        rebuildRequest lastRebuildRequest = {listview::RebuildType::Full, 0};

        unsigned int currentPageSize = 0;
        bool pageLoaded              = true;
        bool focusOnLastItem         = false;
        int scrollTopMargin          = style::margins::big;

        listview::Boundaries boundaries   = listview::Boundaries::Fixed;
        listview::Direction direction     = listview::Direction::Bottom;
        listview::Orientation orientation = listview::Orientation::TopBottom;

        virtual void addItemsOnPage();

        bool requestCompleteData   = false;
        bool requestFullListRender = false;
        bool renderFullList();
        void checkFullRenderRequirement();

        void prepareFullRebuild();
        void prepareOnOffsetRebuild(unsigned int dataOffset);
        void prepareInPlaceRebuild();
        void prepareOnPageElementRebuild(unsigned int dataOffset);

        void setFocus();
        void refresh();
        void resizeWithScroll();
        void recalculateStartIndex();
        void fillFirstPage();
        void setStartIndex();
        void recalculateOnBoxRequestedResize();
        void setFocusOnElement(unsigned int elementNumber);
        [[nodiscard]] unsigned int getFocusItemIndex();
        /// Default empty list to inform that there is no elements - callback should be override in applications
        void onElementsCountChanged();
        unsigned int calculateMaxItemsOnPage();
        unsigned int calculateLimit(listview::Direction value = listview::Direction::Bottom);
        [[nodiscard]] Order getOrderFromDirection() const noexcept;
        [[nodiscard]] Order getOppositeOrderFromDirection() const noexcept;
        virtual bool requestNextPage();
        virtual bool requestPreviousPage();
        void setup(listview::RebuildType rebuildType, unsigned int dataOffset = 0);

      public:
        ListView();
        ListView(Item *parent,
                 unsigned int x,
                 unsigned int y,
                 unsigned int w,
                 unsigned int h,
                 std::shared_ptr<ListItemProvider> prov,
                 listview::ScrollBarType scrollType = listview::ScrollBarType::Proportional);
        ~ListView();

        void setElementsCount(unsigned int count);
        void setProvider(std::shared_ptr<ListItemProvider> provider);

        void rebuildList(listview::RebuildType rebuildType = listview::RebuildType::Full,
                         unsigned int dataOffset           = 0,
                         bool forceRebuild                 = false);
        /// In case of elements count change there can be a need to resend request in case of having one async query for
        /// count and records.
        void reSendLastRebuildRequest();
        /// Callback to be called on rebuild preparation - in example to on demand clear provider data.
        std::function<void()> prepareRebuildCallback;

        void reset();
        void clear();
        void onClose();

        std::shared_ptr<ListItemProvider> getProvider();
        void setOrientation(listview::Orientation value);
        void setBoundaries(listview::Boundaries value);
        void setScrollTopMargin(int value);
        void setAlignment(const Alignment &value) override;
        void onProviderDataUpdate();

        [[nodiscard]] bool isEmpty() const noexcept;
        std::function<void()> emptyListCallback;
        std::function<void()> notEmptyListCallback;

        // virtual methods from Item
        bool onInput(const InputEvent &inputEvent) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
        auto handleRequestResize(const Item *, unsigned short request_w, unsigned short request_h) -> Size override;
    };

} /* namespace gui */
