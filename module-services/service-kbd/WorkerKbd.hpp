/*
 * WorkerKbd.hpp
 *
 *  Created on: May 31, 2019
 *      Author: kuba
 */

#ifndef MODULE_SERVICES_SERVICE_KBD_WORKERKBD_HPP_
#define MODULE_SERVICES_SERVICE_KBD_WORKERKBD_HPP_

#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "Service/Worker.hpp"

class WorkerKbd : public sys::Worker
{

public:
	WorkerKbd( sys::Service* service ) : sys::Worker( service ) {};
	// ~WorkerKbd();


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
	bool handleMessage( uint32_t queueID );
};


#endif /* MODULE_SERVICES_SERVICE_KBD_WORKERKBD_HPP_ */
