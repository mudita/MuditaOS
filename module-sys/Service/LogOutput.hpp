//
// Created by mati on 12.03.19.
//

#ifndef DBUS_LOGOUTPUT_HPP
#define DBUS_LOGOUTPUT_HPP


#include <string>


#ifdef TARGET_Linux
#include <iostream>
#endif



class LogOutput
{
    public:
    static void Output(const std::string &name)
    {

#if  defined(TARGET_Linux)
        std::cout << name << "\n";
#endif
    }
};

#endif //DBUS_LOGOUTPUT_HPP
