// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

template <typename ItemPolicy> class StatusBarWidgetBase : public ItemPolicy
{
  public:
    using ItemPolicy::ItemPolicy;
    virtual ~StatusBarWidgetBase() = default;

    virtual void show()
    {
        ItemPolicy::setVisible(true);
    }

    virtual void hide()
    {
        ItemPolicy::setVisible(false);
    }

    virtual bool isVisible()
    {
        return ItemPolicy::visible;
    }
};
