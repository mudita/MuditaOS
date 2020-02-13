/*
 * @file Worker.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 30 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_SYS_SERVICE_WORKER_HPP_
#define MODULE_SYS_SERVICE_WORKER_HPP_

#include <memory>
#include <string>

#include "Service.hpp"

namespace sys {

class WorkerQueueInfo {
public:
  const char *name;
  int elementSize;
  int length;
};

struct WorkerCommand {
	uint32_t command = 0;
	uint32_t* data = NULL;
};

/*
 * @brief Worker is a wrapper for freeRTOS task used to separate sysmanager environment from
 * the rest of the system. Its purpose is to handle asynchronous events like IRQ or timers.
 * Flow of creating worker is as follows:
 * - create new Worker object and provide pointer to the service that owns the worker,
 * - call init method and provide list of parameters to create queues. Those queues can be later
 * used to wake up the worker.
 * - call run method to start the worker.
 * Flow for closing the worker is as follows:
 * - call stop method - task will send a close confirmation to the service after exiting its main loop
 * - call deinit to destroy all queues ued by the worker
 * - delete the object.
 *
 */
class Worker {
protected:
	const static uint32_t SERVICE_QUEUE_LENGTH = 10;
	const static uint32_t SERVICE_QUEUE_SIZE = sizeof( WorkerCommand );
    const std::string SERVICE_QUEUE_NAME = "ServiceQueue";

    sys::Service* service;
	//queue used by service to send commands to service.
	xQueueHandle serviceQueue;
	std::vector<xQueueHandle> queues;
	QueueSetHandle_t queueSet;
    std::map<xQueueHandle, std::string> queueNameMap;
    xTaskHandle taskHandle;

    friend void workerTaskFunction(void *ptr);

  public:
	Worker( sys::Service* service );
	virtual ~Worker();

	/**
	 * This function is responsible for creating all queues provided in the constructor.
	 * When all queues are created this method creates set of queues.
	 */
	virtual bool init( std::list<WorkerQueueInfo> queues = std::list<WorkerQueueInfo>());
	virtual bool deinit();
	/**
	 * This method starts RTOS thread that waits for incomming queue events.
	 */
	virtual bool run();
	/**
	 * Sends stop command to worker.
	 */
	virtual bool stop();
	/**
	 * This method is called from thread when new message arrives in queue.
	 * @param queueID Index of the queue in the queues vector.
	 */
	virtual bool handleMessage( uint32_t queueID );
	/**
	 * @brief Sends command and pointer to data to worker
	 */
	virtual bool send( uint32_t cmd, uint32_t* data );
    /**
     * @brief Returns handle to queue by its name
     */
    virtual xQueueHandle getQueueByName(std::string);
};

} /* namespace sys */

#endif /* MODULE_SYS_SERVICE_WORKER_HPP_ */
