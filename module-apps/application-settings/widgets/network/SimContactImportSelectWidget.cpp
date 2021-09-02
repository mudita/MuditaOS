// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimContactImportSelectWidget.hpp"

namespace gui
{
    SimContactImportSelectWidget::SimContactImportSelectWidget(
        const std::string &contactName,
        const std::function<void(const UTF8 &text)> &bottomBarTemporaryMode,
        const std::function<void()> &bottomBarRestoreFromTemporaryMode)
    {
        setEdges(RectangleEdge::None);
        setMinimumSize(style::window::default_body_width, style::window::label::big_h);
        setMargins(gui::Margins(style::widgets::leftMargin, style::margins::big, 0, 0));

        checkBoxWithLabel = new gui::CheckBoxWithLabel(this,
                                                       0,
                                                       0,
                                                       0,
                                                       0,
                                                       contactName,
                                                       bottomBarTemporaryMode,
                                                       bottomBarRestoreFromTemporaryMode,
                                                       BottomBar::Side::LEFT);

        inputCallback = [&]([[maybe_unused]] Item &item, const InputEvent &event) {
            return checkBoxWithLabel->onInput(event);
        };

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? checkBoxWithLabel : nullptr);
            return true;
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            checkBoxWithLabel->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    bool SimContactImportSelectWidget::isChecked()
    {
        return checkBoxWithLabel->isChecked();
    }
} /* namespace gui */
