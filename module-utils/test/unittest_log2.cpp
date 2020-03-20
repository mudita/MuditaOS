/*
 * unittest_log2.cpp
 *
 *  Created on: 29 kwi 2019
 *      Author: robert
 */

#include "segger/log/log.hpp"

#include <iostream>
#include <string>

using namespace std;

int log_test_function()
{

    LOG_TRACE("This should be blue");
    LOG_DEBUG("This should be navy blue");
    LOG_INFO("This should be green");
    LOG_WARN("This should be yellow");
    LOG_ERROR("This should be red");
    LOG_FATAL("This should be purple");

    return 0;
}
