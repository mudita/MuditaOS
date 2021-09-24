// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SoundFileListItem.hpp"

#include <common/data/StyleCommon.hpp>

using namespace gui;

SoundFileListItem::SoundFileListItem(AbstractSettingsModel<std::string> &model,
                                     app::bell_settings::AlarmSoundType soundType,
                                     typename ModelDelegateSpinner<app::bell_settings::AlarmSoundDelegate>::Range range,
                                     const std::string &topDescription)
    : BellSideListItemWithCallbacks(topDescription)
{
    spinner = new ModelDelegateSpinner<app::bell_settings::AlarmSoundDelegate>(std::move(range), Boundaries::Fixed);
    spinner->setMaximumSize(::style::bell_base_layout::w, ::style::bell_base_layout::h);
    spinner->setFont(bell_style::font_center);
    spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
    spinner->setFocusEdges(RectangleEdge::None);
    body->getCenterBox()->addWidget(spinner);

    getValue = [&model, this]() { model.setValue(spinner->getCurrentValue().path.filename()); };
    setValue = [&model, soundType, this]() {
        spinner->setCurrentValue(app::bell_settings::AlarmSoundDelegate{soundType, model.getValue()});
    };
}
