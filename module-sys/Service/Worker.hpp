/*
 * @file Worker.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 29 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_SYS_SERVICE_WORKER_HPP_
#define MODULE_SYS_SERVICE_WORKER_HPP_

#include "Service.hpp"

namespace sys {

/*
 *
 */
class Worker {
protected:
	//Pointer to the service that controls this worker
	sys::Service* owner;
public:
	Worker( sys::Service* owner );
	virtual ~Worker();

	virtual int execute();
};

} /* namespace sys */

#endif /* MODULE_SYS_SERVICE_WORKER_HPP_ */
