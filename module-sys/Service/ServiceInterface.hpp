// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <stdexcept>
#include <utility>

namespace sys
{
    class Service;
}

namespace service
{
    /// settings interface for elements which reach to Service with straight ptr
    /// should be initialized for user element in InitHandler
    class Interface
    {
      private:
        std::weak_ptr<sys::Service> app;

      protected:
        std::shared_ptr<sys::Service> getApp()
        {
            if (isValid()) {
                return app.lock();
            }
            throw std::runtime_error("no app");
        }

      public:
        explicit Interface(std::weak_ptr<sys::Service> app) : app(std::move(app))
        {}
        Interface() = default;
        bool isValid() const
        {
            return !app.expired() && app.lock() != nullptr;
        }
    };
} // namespace service
