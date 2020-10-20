// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_SERVICES_SERVICE_EINK_EINKMESSAGE_HPP_
#define MODULE_SERVICES_SERVICE_EINK_EINKMESSAGE_HPP_

#include "Service/Message.hpp"
#include "MessageType.hpp"

namespace seink
{

    /*
     * @brief Template for all messages that go to gui service
     */
    class EinkMessage : public sys::DataMessage
    {
      public:
        EinkMessage(MessageType messageType) : sys::DataMessage(messageType){};
        virtual ~EinkMessage(){};
    };

} /* namespace seink */

#endif /* MODULE_SERVICES_SERVICE_EINK_EINKMESSAGE_HPP_ */
