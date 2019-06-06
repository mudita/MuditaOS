/*
 * @file MessageType.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef SOURCE_MESSAGETYPE_HPP_
#define SOURCE_MESSAGETYPE_HPP_

enum class MessageType {

	MessageTypeUninitialized = 0,
	//eink messages
	EinkStateRequest, //message is used to pull status of the eink. If eink is ready to display image
	EinkImageData, //message with pointer to the image data for displaying
	EinkTemperatureUpdate, //message sent from timer handler. Message forces service to update temperature measured by the eink.

	//gui messages
	GUICommands, // list of rendering commands
	GUIFocusInfo, // information about application that gained focus
	GUIDisplayReady, //message that informs gui service that service controlling display device is ready for new buffer data.
	GUIRenderingFinished, //Message is sent from the worker when all rendering is finished.

	//keyboard messages
	KBDKeyEvent
};



#endif /* SOURCE_MESSAGETYPE_HPP_ */
