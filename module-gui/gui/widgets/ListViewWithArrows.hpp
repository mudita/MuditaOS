// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "ListViewEngine.hpp"
#include "ListItemProvider.hpp"
#include "ThreeBox.hpp"
#include <utf8/UTF8.hpp>

namespace gui
{
    class ImageBox;
    class TextFixedSize;

    class ListViewWithArrows : public Rect, public ListViewEngine
    {
      protected:
        VBox *widgetBody                         = nullptr;
        VThreeBox<VBox, VBox, VBox> *listOverlay = nullptr;
        ImageBox *arrowTop                       = nullptr;
        ImageBox *arrowDown                      = nullptr;
        TextFixedSize *titleBody                 = nullptr;

        void createLayout();
        void applyScrollCallbacks();
        void setFocus() override;

      public:
        ListViewWithArrows(Item *parent,
                           unsigned int x,
                           unsigned int y,
                           unsigned int w,
                           unsigned int h,
                           std::shared_ptr<ListItemProvider> prov);

        void setListTitle(const std::string &title);
        void setListTitleFont(const UTF8 &fontName);
        void applySizeRestrictions(unsigned int w, unsigned int h, unsigned int outerLayoutsH, int outerLayoutsMargin);

        void setAlignment(const Alignment &value) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };
} // namespace gui
