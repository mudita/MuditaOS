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

    class ListViewScroll : public Rect
    {
      public:
        ListViewScroll(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

        bool shouldShowScroll(int listPageSize, int elementsCount);
        void update(int startIndex, int listPageSize, int elementsCount, int topMargin);
    };

    class ListView : public Rect
    {
      protected:
        unsigned int startIndex                    = 0;
        unsigned int storedFocusIndex              = 0;
        unsigned int elementsCount                 = 1;
        std::shared_ptr<ListItemProvider> provider = nullptr;
        VBox *body                                 = nullptr;
        ListViewScroll *scroll                     = nullptr;

        unsigned int currentPageSize = 0;
        bool pageLoaded              = false;
        bool focusOnLastItem         = false;
        int scrollTopMargin          = style::margins::big;

        style::listview::Type listType       = style::listview::Type::TopDown;
        style::listview::Direction direction = style::listview::Direction::Bottom;

        void clearItems();
        virtual void addItemsOnPage();
        void setFocus();
        void refresh();
        void resizeWithScroll();
        void recalculateStartIndex();
        void checkFirstPage();
        unsigned int calculateMaxItemsOnPage();
        unsigned int calculateLimit();
        Order getOrderFromDirection();
        virtual bool requestNextPage();
        virtual bool requestPreviousPage();
        void setup(style::listview::RebuildType rebuildType);

      public:
        ListView();
        ListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, std::shared_ptr<ListItemProvider> prov);
        ~ListView();

        void setElementsCount(int count);
        void setProvider(std::shared_ptr<ListItemProvider> provider);
        void rebuildList(style::listview::RebuildType rebuildType = style::listview::RebuildType::Full);
        void clear();
        std::shared_ptr<ListItemProvider> getProvider();
        void setListViewType(style::listview::Type type);
        void setScrollTopMargin(int value);
        void setAlignment(const Alignment &value) override;
        void onProviderDataUpdate();

        // virtual methods from Item
        std::list<DrawCommand *> buildDrawList() override;
        bool onInput(const InputEvent &inputEvent) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
