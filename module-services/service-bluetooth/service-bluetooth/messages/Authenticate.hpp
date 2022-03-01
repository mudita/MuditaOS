// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace bluetooth
{
    enum class AuthenticateType
    {
        Pin,
        Passkey,
        PairCancel
    };
}

namespace message::bluetooth
{

    class RequestAuthenticate : public BluetoothMessage
    {
      private:
        Devicei device;
        ::bluetooth::AuthenticateType type;

      public:
        RequestAuthenticate(const Devicei &dev, ::bluetooth::AuthenticateType type) : device(dev), type(type)
        {}

        [[nodiscard]] auto getDevice()
        {
            return device;
        }

        [[nodiscard]] auto getAuthenticateType()
        {
            return type;
        }
    };

    class ResponseAuthenticate : public BluetoothMessage
    {
      private:
        Devicei device;

      protected:
        explicit ResponseAuthenticate(const Devicei &dev) : device(dev)
        {}

      public:
        [[nodiscard]] auto getDevice()
        {
            return device;
        }
    };

    class ResponseAuthenticatePin : public ResponseAuthenticate
    {
      private:
        std::string pin;

      public:
        ResponseAuthenticatePin(std::string pin, const Devicei &dev) : ResponseAuthenticate(dev), pin(std::move(pin))
        {}

        [[nodiscard]] auto getPin() const -> std::string
        {
            return pin;
        }
    };

    class ResponseAuthenticatePasskey : public ResponseAuthenticate
    {
      private:
        std::string passkey;

      public:
        ResponseAuthenticatePasskey(std::string passkey, const Devicei &dev)
            : ResponseAuthenticate(dev), passkey(std::move(passkey))
        {}

        [[nodiscard]] auto getPasskey() const -> std::string
        {
            return passkey;
        }
    };

    class ResponseAuthenticatePairCancel : public ResponseAuthenticate
    {
      private:
        bool pairApproved = false;

      public:
        ResponseAuthenticatePairCancel(bool pairApproved, const Devicei &dev)
            : ResponseAuthenticate(dev), pairApproved(pairApproved)
        {}

        [[nodiscard]] auto getPairApproved() const -> bool
        {
            return pairApproved;
        }
    };

} // namespace message::bluetooth
