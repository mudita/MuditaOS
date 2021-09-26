// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IntervalChimePresenter.hpp"

namespace app::meditation
{
    IntervalChimePresenter ::IntervalChimePresenter(app::ApplicationCommon *app)
        : app{app}, model{std::make_shared<IntervalChimeModel>()}
    {}

    void IntervalChimePresenter::set(MeditationItem &item)
    {
        model->createData();
        model->setData(item);
    }

    void IntervalChimePresenter::get(MeditationItem &item)
    {
        MeditationItem *p = model->getData();
        if (p != nullptr) {
            item.copyFrom(p);
        }
    }

    auto IntervalChimePresenter::getProvider() -> std::shared_ptr<gui::ListItemProvider>
    {
        return model;
    }
} // namespace app::meditation
