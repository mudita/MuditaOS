/*
 * @file Worker.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 30 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <string.h>

extern "C" {
	#include "FreeRTOS.h"
	#include "task.h"
}

//module-sys
#include "Worker.hpp"

namespace sys {

static void workerTaskFunction( void* ptr ) {
	Worker* worker = reinterpret_cast<Worker*>(ptr);
	QueueSetMemberHandle_t activeMember;
	std::vector<xQueueHandle> queues = worker->getQueues();

	while (1)
	{
		activeMember = xQueueSelectFromSet( worker->getSet(), portMAX_DELAY);
		//find id of the queue that was activated

		for( uint32_t i=0; i<queues.size(); i++ ) {
			if( queues[i] == activeMember ) {
				worker->handleMessage( i );
			}
		}
	}
}



Worker::Worker( sys::Service* service ) : service {service }, serviceQueue{ NULL }, queueSet{ NULL }, taskHandle{ NULL } {
}

Worker::~Worker() {
}

bool Worker::init( std::list<WorkerQueueInfo> queuesList ) {

	//initial value is because there is always a queue to communicate with service
	uint32_t setSize = SERVICE_QUEUE_LENGTH;

	//iterate over all entries in the list of queues and summarize queue sizes
	for( auto wqi : queuesList ) {
		setSize += wqi.length;
	}

	//create set of queues
	queueSet = xQueueCreateSet( setSize );
	if( queueSet == NULL )
		return false;

	//create and add all queues to the set. First service queue is created.
	serviceQueue = xQueueCreate(SERVICE_QUEUE_LENGTH, SERVICE_QUEUE_SIZE );
	if(serviceQueue == nullptr){
        deinit();
        return false;
	}
	queues.push_back( serviceQueue );

	//create and add all queues provided from service
	for( auto wqi : queuesList ) {
		auto q = xQueueCreate(wqi.length, wqi.elementSize );
		if( q == NULL ) {
//			LOG_FATAL("xQueueCreate %s failed", wqi.name.c_str());
			deinit();
			return false;
		}
		queues.push_back( q );
	}

	//iterate over all queues and add them to set
	for( uint32_t i=0; i<queues.size(); ++i ) {

		if(xQueueAddToSet( queues[i], queueSet ) != pdPASS)
		{
//			LOG_FATAL("xQueueAddToSet %d failed", i);
			deinit();
			return false;
		}
	}

	return true;
}
bool Worker::deinit() {

    vTaskDelete(taskHandle);

	//for all queues - remove from set and delete queue
	for( auto q : queues ) {
		//remove queues from set
		xQueueRemoveFromSet( q, queueSet );
		//delete queue
		vQueueDelete( q );
	}
	queues.clear();

	//delete queues set
	vQueueDelete((QueueHandle_t) queueSet );
	queueSet = NULL;
	taskHandle = NULL;

	return true;
};
/**
 * This method starts RTOS thread that waits for incomming queue events.
 */
bool Worker::run() {
    static int workerCount = 0;
    std::string workerName = service->GetName()+"_w" + std::to_string(workerCount);
    BaseType_t task_error = xTaskCreate(
            workerTaskFunction,
            workerName.c_str(),
            512,
            this,
            service->GetPriority(),
            &taskHandle);
    if ( task_error != pdPASS)
    {
        LOG_ERROR("Failed to start the task");
        return false;
    }
    return true;
}

bool Worker::stop() {
	return send( 0,NULL );

}


bool Worker::handleMessage( uint32_t queueID ) {

	QueueHandle_t queue = queues[queueID];

	//service queue
	if( queueID == 0 ) {
		WorkerCommand wcmd;
		if( xQueueReceive(queue, &wcmd, 0 ) != pdTRUE ) {
			return false;
		}
		wcmd.command = 1;
		//place some code here to handle messages from service

	}
	return true;
}

xQueueHandle Worker::getQueue() {
	return serviceQueue;
}

bool Worker::send( uint32_t cmd, uint32_t* data ) {
	if( serviceQueue != NULL ) {
		WorkerCommand wcmd {cmd, data };
		if( xQueueSend( serviceQueue, &wcmd, portMAX_DELAY ) == pdTRUE )
			return true;
	}
	return false;
}

} /* namespace sys */
