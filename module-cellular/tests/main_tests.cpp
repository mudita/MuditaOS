
/*
 * @file main_tests.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 14.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "cellular/linux_cellular.hpp"
#include <iostream>
#include <memory>
#include <cstring>
#include <unistd.h>


#include "catch.hpp"


TEST_CASE("Cellular module main test body")
{
    auto cellular = bsp::Cellular::Create("/dev/ttyUSB0");

    while(1){

        char buff[] = "AT\r\n";
        std::cout << "BytesWritten: " <<  cellular->Write(buff,strlen(buff)) << "\n";

        if(cellular->Wait(30000) == 1){
            //read from
            char buff[1024];
            auto bytesWritten = cellular->Read(buff,sizeof buff);
            buff[bytesWritten] = '\0';
            std::cout << buff << "\n";

        }
        else{
            std::cout << "timeout\n";
        }
    }


}

