/*
 * WorkerEvent.hpp
 *
 *  Created on: May 31, 2019
 *      Author: kuba
 */

#ifndef MODULE_SERVICES_SERVICE_KBD_WorkerEvent_HPP_
#define MODULE_SERVICES_SERVICE_KBD_WorkerEvent_HPP_


#include "bsp/keyboard/key_codes.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "Service/Worker.hpp"

#include "EventManager.hpp"

#include "bsp/common.hpp"
#include "bsp/keyboard/key_codes.hpp"

struct KeyState{
	uint8_t event;
	uint8_t code;
};

enum class WorkerEventQueues
{
    queueService = 0,
    queueKeyboardIRQ,
    queueBattery,
    queueRTC,
    queueHarness,
};

class WorkerEvent : public sys::Worker
{
private:
	/**
	 * @brief This method is responsible for catch and process keyboard event.
	 * @param event key event
	 * @param code key code
	 * @note It sends message to service if event is processed succesfully.
	 */
	void processKeyEvent( bsp::KeyEvents event, bsp::KeyCodes code );
	/**
	 * @brief list of keys with long press enabled. First item is key code, second is long press time.
	 */
	std::map<uint32_t, uint32_t> longPressParamsList;
	bool longPressTaskEnabled = false;
	bsp::KeyEvents lastState = bsp::KeyEvents::Released;
	bsp::KeyCodes lastPressed = static_cast<bsp::KeyCodes>(0);
public:
	WorkerEvent( sys::Service* service ) : sys::Worker( service ) {};
	/**
	 * This function is responsible for creating all queues provided in the constructor.
	 * When all queues are created this method creates set of queues.
	 */
	virtual bool init( std::list<sys::WorkerQueueInfo> queues );
	virtual bool deinit();

	/**
	 * This method is called from thread when new message arrives in queue.
	 * @param queueID Index of the queue in the queues vector.
	*/
	bool handleMessage( uint32_t queueID ) override final;

};


#endif /* MODULE_SERVICES_SERVICE_KBD_WorkerEvent_HPP_ */
