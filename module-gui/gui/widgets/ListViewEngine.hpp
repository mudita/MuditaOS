// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ListItemProvider.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    namespace listview
    {
        inline constexpr auto nPos = std::numeric_limits<unsigned>::max();

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
            OnPageElement, ///< OnPageElement rebuild - focus on provided element index and calculated page.
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

        /// Possible page fill types
        enum class FetchType
        {
            Filled, ///< Filled - list always fetch object from provider as long it has space.
            Fixed   ///< Fixed - lists fetches objects from provider accordinly to fixed pages sizes.
        };

        enum class Orientation
        {
            TopBottom,
            BottomTop
        };

    } // namespace listview

    struct ListViewScrollSetupData
    {
        const unsigned storedStartIndex;
        const unsigned currentPage;
        const unsigned pagesCount;
    };

    struct ListViewScrollUpdateData
    {
        const unsigned startIndex;
        const unsigned listPageSize;
        const unsigned elementsCount;
        const unsigned elementMinimalSpaceRequired;
        const listview::Direction direction;
        const Boundaries boundaries;
    };

    class ListItemProvider;

    using rebuildRequest = std::pair<listview::RebuildType, unsigned>;

    class ListViewEngine
    {
      protected:
        explicit ListViewEngine(std::shared_ptr<ListItemProvider> prov);
        virtual ~ListViewEngine();

        /// First requested index from provider
        unsigned startIndex = 0;
        void setStartIndex();
        /// Recalculating startIndex if list internal conditions (i.e. size has changed).
        void recalculateStartIndex();
        /// Calculate max items on page based on provided minimal item in axis size from provider.
        /// That method is used when full list render can be omitted.
        unsigned calculateMaxItemsOnPage();
        /// Calculates request elements count to full next request page with small excess
        unsigned calculateLimit(listview::Direction value = listview::Direction::Bottom);
        /// Recalculate startIndex on body resize requests
        void recalculateOnBoxRequestedResize();

        /// Request next page based on calculated offset and limit
        virtual bool requestNextPage();
        /// Request previous based on calculated offset and limit
        virtual bool requestPreviousPage();
        /// Request data to fillFirst list page - used in some conditions (i.e items are various item in axis sizes)
        void fillFirstPage();

        /// Stored index of focused element before new data request
        unsigned storedFocusIndex = listview::nPos;
        [[nodiscard]] unsigned getFocusItemIndex();

        /// Total provider elements count
        unsigned elementsCount = listview::nPos;
        void setElementsCount(unsigned count);
        void onElementsCountChanged(unsigned count);
        bool shouldCallEmptyListCallbacks = false;
        void checkEmptyListCallbacks();

        /// Data model provider
        std::shared_ptr<ListItemProvider> provider = nullptr;

        /// Count of elements displayed on current page
        unsigned currentPageSize = 0;
        /// Flag indicating that page has been loaded
        bool pageLoaded = true;
        /// Handle focusing method
        virtual void setFocus();
        /// Flag indicating that focus after rebuild has to be on last displayed element
        bool focusOnLastItem = false;

        /// Layout element to display received data
        BoxLayout *body = nullptr;

        /// Scroll handling callbacks
        std::function<void(const ListViewScrollUpdateData &data)> updateScrollCallback;
        std::function<void(const ListViewScrollSetupData &data)> setupScrollCallback;
        std::function<void()> resizeScrollCallback;

        /// Main loop method
        void refresh();
        /// Adding elements from provider to page
        virtual void addItemsOnPage();

        /// Pending rebuild requests
        std::list<rebuildRequest> rebuildRequests;
        /// Stored last executed rebuildRequest
        rebuildRequest lastRebuildRequest = {listview::RebuildType::Full, 0};
        /// Setup method for list rebuild request
        void setup(listview::RebuildType rebuildType, unsigned dataOffset = 0);
        void prepareFullRebuild();
        void prepareOnOffsetRebuild(unsigned dataOffset);
        void prepareInPlaceRebuild();
        void prepareOnPageElementRebuild(unsigned dataOffset);

        /// List boundaries types
        Boundaries boundaries = Boundaries::Fixed;
        /// List current movement direction
        listview::Direction direction = listview::Direction::Bottom;
        /// List orientation (from which end element will start to load)
        listview::Orientation orientation = listview::Orientation::TopBottom;
        /// List fill type
        listview::FetchType fetchType = listview::FetchType::Filled;

        /// Flag to indicate full data request completed
        bool requestCompleteData = false;
        /// Flag to indicate full list render (with all provider data) completed
        bool requestFullListRender = false;
        /// Full list render method
        bool renderFullList();
        /// Checking requirements for full list render
        std::function<void()> checkFullRenderRequirementCallback;

        /// Methods to translate direction to request order
        [[nodiscard]] Order getOrderFromDirection() const noexcept;
        [[nodiscard]] Order getOppositeOrderFromDirection() const noexcept;

      public:
        /// set data model provider method
        void setProvider(std::shared_ptr<ListItemProvider> provider);

        /// send list rebuild request
        void rebuildList(listview::RebuildType rebuildType = listview::RebuildType::Full,
                         unsigned dataOffset               = 0,
                         bool forceRebuild                 = false);
        /// In case of elements count change there can be a need to resend request in case of having one async query for
        /// count and records.
        void reSendLastRebuildRequest();
        /// Callback to be called on rebuild preparation - in example to on demand clear provider data.
        std::function<void()> prepareRebuildCallback;

        void reset();
        virtual void clear();
        void onClose();

        std::shared_ptr<ListItemProvider> getProvider();
        void setOrientation(listview::Orientation value);
        void setBoundaries(Boundaries value);
        void onProviderDataUpdate();

        /// Empty list handling callbacks
        [[nodiscard]] bool isEmpty() const noexcept;
        std::function<void()> emptyListCallback;
        std::function<void()> notEmptyListCallback;

        /// Update the number of items above the current page
        void updateCountOfElementsAboveCurrentPage();
        /// Callback on update the number of items above the current page
        std::function<void(const unsigned elementsAboveOfCurrentPageCount)> onElementsAboveOfCurrentPageChangeCallback;
    };

} /* namespace gui */
