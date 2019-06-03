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

namespace seink {

enum class MessageType {
	Uninitialized,
	StateRequest, //message is used to pull status of the eink. If eink is ready to display image
	ImageData, //message with pointer to the image data for displaying
	TemperatureUpdate, //message sent from timer handler. Message forces service to update temperature measured by the eink.
};

/*
 * @brief Template for all messages that go to gui service
 */
class EinkMessage: public sys::DataMessage {
public:
	//Identifies type of message within EINK Service
	const MessageType messageType;

	EinkMessage( MessageType messageType ) : sys::DataMessage(), messageType{ messageType } {};
	virtual ~EinkMessage() {};
};

//class WorkerMessage: public EinkMessage {
//protected:
//	uint32_t command;
//public:
//	WorkerMessage( uint32_t cmd ) : EinkMessage{ seink::MessageType::WorkerReport }, command{ cmd } {};
//	virtual ~WorkerMessage() {};
//
//	uint32_t getCommand() { return command; };
//};

} /* namespace seink */


#endif /* MODULE_SERVICES_SERVICE_EINK_EINKMESSAGE_HPP_ */
