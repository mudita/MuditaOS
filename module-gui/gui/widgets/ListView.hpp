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
    class ListItemProvider;

    using rebuildRequest = std::pair<style::listview::RebuildType, unsigned int>;

    struct ListViewScrollUpdateData
    {
        const unsigned int startIndex;
        const unsigned int listPageSize;
        const unsigned int elementsCount;
        const unsigned int elementMinimalHeight;
        const style::listview::Direction direction;
        const style::listview::Boundaries boundaries;
        const int topMargin;
    };

    class ListViewScroll : public Rect
    {
      private:
        unsigned int storedStartIndex = 0;
        unsigned int currentPage      = style::listview::nPos;
        unsigned int pagesCount       = 0;

        void updateProportional(const ListViewScrollUpdateData &data);
        void updateFixed(const ListViewScrollUpdateData &data);
        void updatePreRendered(const ListViewScrollUpdateData &data);

      public:
        style::listview::ScrollBarType type = style::listview::ScrollBarType::None;

        ListViewScroll(Item *parent,
                       unsigned int x,
                       unsigned int y,
                       unsigned int w,
                       unsigned int h,
                       style::listview::ScrollBarType type);

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
        unsigned int storedFocusIndex              = style::listview::nPos;
        unsigned int elementsCount                 = 0;
        std::shared_ptr<ListItemProvider> provider = nullptr;
        VBox *body                                 = nullptr;
        ListViewScroll *scroll                     = nullptr;
        std::list<rebuildRequest> rebuildRequests;
        rebuildRequest lastRebuildRequest = {style::listview::RebuildType::Full, 0};

        unsigned int currentPageSize = 0;
        bool pageLoaded              = true;
        bool focusOnLastItem         = false;
        int scrollTopMargin          = style::margins::big;

        style::listview::Boundaries boundaries   = style::listview::Boundaries::Fixed;
        style::listview::Direction direction     = style::listview::Direction::Bottom;
        style::listview::Orientation orientation = style::listview::Orientation::TopBottom;

        void clearItems();
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
        unsigned int calculateLimit(style::listview::Direction value = style::listview::Direction::Bottom);
        [[nodiscard]] Order getOrderFromDirection() const noexcept;
        [[nodiscard]] Order getOppositeOrderFromDirection() const noexcept;
        virtual bool requestNextPage();
        virtual bool requestPreviousPage();
        void setup(style::listview::RebuildType rebuildType, unsigned int dataOffset = 0);

      public:
        ListView();
        ListView(Item *parent,
                 unsigned int x,
                 unsigned int y,
                 unsigned int w,
                 unsigned int h,
                 std::shared_ptr<ListItemProvider> prov,
                 style::listview::ScrollBarType scrollType = style::listview::ScrollBarType::Proportional);
        ~ListView();

        void setElementsCount(unsigned int count);
        void setProvider(std::shared_ptr<ListItemProvider> provider);

        void rebuildList(style::listview::RebuildType rebuildType = style::listview::RebuildType::Full,
                         unsigned int dataOffset                  = 0,
                         bool forceRebuild                        = false);
        /// In case of elements count change there can be a need to resend request in case of having one async query for
        /// count and records.
        void reSendLastRebuildRequest();
        /// Callback to be called on rebuild preparation - in example to on demand clear provider data.
        std::function<void()> prepareRebuildCallback;

        void clear();
        std::shared_ptr<ListItemProvider> getProvider();
        void setOrientation(style::listview::Orientation value);
        void setBoundaries(style::listview::Boundaries value);
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
