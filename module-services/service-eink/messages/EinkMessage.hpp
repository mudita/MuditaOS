/*
 * @file EinkMessage.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 28 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_SERVICES_SERVICE_EINK_EINKMESSAGE_HPP_
#define MODULE_SERVICES_SERVICE_EINK_EINKMESSAGE_HPP_


#include "Service/Message.hpp"
#include "MessageType.hpp"

namespace seink {

/*
 * @brief Template for all messages that go to gui service
 */
class EinkMessage: public sys::DataMessage {
public:
	EinkMessage( MessageType messageType ) : sys::DataMessage( static_cast<uint32_t>(messageType)) {};
	virtual ~EinkMessage() {};
};

} /* namespace seink */


#endif /* MODULE_SERVICES_SERVICE_EINK_EINKMESSAGE_HPP_ */
