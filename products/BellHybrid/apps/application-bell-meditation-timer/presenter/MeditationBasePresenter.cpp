// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationBasePresenter.hpp"

#include "log.hpp"

namespace app::meditation
{
    MeditationBasePresenter ::MeditationBasePresenter(app::ApplicationCommon *app) : app{app}
    {}

    void MeditationBasePresenter::activate(MeditationItem &item)
    {
        model = std::make_shared<MeditationBaseModel>();
        model->setData(item);

        getView()->updateDisplay();
    }

    void MeditationBasePresenter::request(MeditationItem &item)
    {
        MeditationItem *p = model->getData();
        if (p != nullptr) {
            item.copyFrom(p);
        }
    }
} // namespace app::meditation
