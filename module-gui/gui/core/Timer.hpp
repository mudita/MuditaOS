// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace gui
{
    using ms = unsigned int;

    /// gui::Timer helper class to be used as passed to gui::Item `onTimer` function
    /// this interface should be connected to real timer build with system Timer
    /// all gui timers should be `Coarse` timers
    class Timer
    {
      public:
        virtual ~Timer() = default;
        enum Type
        {
            Single,   /// single run timer
            Continous /// free running timers
        };
        virtual void start()              = 0;
        virtual void stop()               = 0;
        virtual void reset()              = 0;
        virtual void setInterval(ms time) = 0;
        [[nodiscard]] auto interval() const -> ms;

      protected:
        Type type = Single;

      public:
        [[nodiscard]] auto getType() const -> Type
        {
            return type;
        }
        virtual void setType(Type new_type)
        {
            type = new_type;
        };
    };
}; // namespace gui
