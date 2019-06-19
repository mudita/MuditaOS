
/*
 * @file main_tests.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 14.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "Modem/MuxDaemon.hpp"
#include <iostream>
#include <memory>
#include <cstring>
#include <unistd.h>


#include "catch.hpp"


TEST_CASE("Cellular module main test body")
{
    MuxDaemon daemon;

    daemon.Start();
}

