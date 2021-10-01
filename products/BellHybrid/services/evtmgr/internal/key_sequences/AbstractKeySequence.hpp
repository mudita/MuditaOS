// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/key_input/RawKey.hpp>

#include <functional>

class AbstractKeySequence
{
    template <class Cb, class... Args> static void invoke(Cb &&callback, Args &&...args)
    {
        if (callback) {
            callback(args...);
        }
    }

  public:
    using Callback = std::function<void(const AbstractKeySequence &)>;

    virtual ~AbstractKeySequence()          = default;
    virtual void process(const RawKey &key) = 0;

    void trigger() const
    {
        invoke(onTriggered, *this);
    }

    void ready() const
    {
        invoke(onReady, *this);
    }

    void idle() const
    {
        invoke(onIdle, *this);
    }

    void abort() const
    {
        invoke(onAbort, *this);
    }

    void action() const
    {
        invoke(onAction);
    }

    void setCallbacks(Callback &&triggered, Callback &&ready, Callback &&idle, Callback &&abort)
    {
        onTriggered = triggered;
        onReady     = ready;
        onIdle      = idle;
        onAbort     = abort;
    }

    /// Used to inject custom action into key sequence
    std::function<void()> onAction;

  private:
    Callback onTriggered;
    Callback onReady;
    Callback onIdle;
    Callback onAbort;
};
