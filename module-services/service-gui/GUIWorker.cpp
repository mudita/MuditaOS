/*
 * @file GUIWorker.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 31 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "GUIWorker.hpp"

namespace sgui {

GUIWorker::GUIWorker( ServiceGUI* service ) : Worker( service ) {

}

GUIWorker::~GUIWorker() {
}

bool GUIWorker::handleMessage( uint32_t queueID ) {
	return true;
}

} /* namespace sgui */
