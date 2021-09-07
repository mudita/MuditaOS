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
    /// Proxy class for classes using Service raw pointer
    /// - initialized in InitHandler
    /// - provides weak bounding with Service (to avoid infinite shared ptr loop)
    /// - used to provide Class <-> System interface
    /// - invalid ServiceProxy will throw runtime error instead of crash on invalid Service* ptr
    class ServiceProxy
    {
      private:
        std::weak_ptr<sys::Service> service;

      protected:
        [[nodiscard]] std::shared_ptr<sys::Service> getService()
        {
            if (auto val = service.lock(); val != nullptr) {
                return val;
            }
            throw std::runtime_error("no service");
        }

      public:
        explicit ServiceProxy(std::weak_ptr<sys::Service> service) : service(std::move(service))
        {}
        ServiceProxy()          = default;
        virtual ~ServiceProxy() = default;
        [[nodiscard]] bool isValid() const noexcept
        {
            return !service.expired();
        }
    };
} // namespace service
