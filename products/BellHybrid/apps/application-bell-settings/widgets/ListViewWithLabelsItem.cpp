// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ListViewWithLabelsItem.hpp"

namespace gui
{
    ListViewWithLabelsItem::ListViewWithLabelsItem(const std::string &description,
                                                   std::shared_ptr<ListItemProvider> prov)
        : gui::BellSideListItemWithCallbacks(description)
    {
        //        auto duoBox = new VBox(body->getCenterBox());
        //        duoBox->setEdges(RectangleEdge::None);
        //        duoBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        //        duoBox->setMaximumSize(::style::bell_base_layout::w, ::style::bell_base_layout::h);

        list = new ListViewWithLabels(body->getCenterBox(), 0, 0, 400, 200, std::move(prov));
        list->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        list->applySizeRestrictions(400, 200, 0, 0);
        setFocusItem(list);
        list->setVisible(true);

        body->firstBox->setVisible(false);
        body->lastBox->setVisible(false);
        body->resize();

        getValue = []() { LOG_ERROR("getValue called!"); };
        setValue = [this]() {
            LOG_ERROR("setValue called!");
            list->rebuildList(listview::RebuildType::InPlace);
        };

        //        auto txt = new TextFixedSize(duoBox);
        //        txt->setMinimumSize(style::bell_base_layout::w, 2 * (style::bell_base_layout::center_layout_h / 3));
        //
        //        txt->setText("Siema");
        //        txt->setVisible(true);

        // TODO add callbacks
    }
} // namespace gui
