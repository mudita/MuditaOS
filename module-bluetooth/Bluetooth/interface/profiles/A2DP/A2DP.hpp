#pragma once

#include "FreeRTOS.h"
#include "Profile.hpp"
#include "Service/Service.hpp"
#include "queue.h"
#include <memory>
namespace Bt
{
    class A2DP : public Profile
    {
      public:
        A2DP();
        ~A2DP() override;

        A2DP(A2DP &other);
        auto operator=(A2DP rhs) -> A2DP &;
        A2DP(A2DP &&other) noexcept;
        auto operator=(A2DP &&other) noexcept -> A2DP &;

        auto init() -> Error override;
        void setDeviceAddress(uint8_t *addr) override;
        void setOwnerService(sys::Service *service);
        void start();
        void stop();

      private:
        class A2DPImpl;
        std::unique_ptr<A2DPImpl> pimpl;
    };
} // namespace Bt
