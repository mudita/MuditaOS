// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <log/log.hpp>

namespace gui::status_bar
{
    class SIM;
    class Time;
} // namespace gui::status_bar

class StatusBarVisitor
{
    void logError() const
    {
        LOG_ERROR("Invalid widget visited");
    }

  public:
    virtual void visit([[maybe_unused]] gui::status_bar::SIM &widget) const
    {
        logError();
    }
    virtual void visit([[maybe_unused]] gui::status_bar::Time &widget) const
    {
        logError();
    }
    virtual ~StatusBarVisitor() = default;
};

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

    virtual void acceptStatusBarVisitor([[maybe_unused]] StatusBarVisitor &visitor)
    {
        LOG_ERROR("Invalid visitor");
    }
};
