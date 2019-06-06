
/*
 * @file DBServiceAPI.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 06.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_DBSERVICEAPI_HPP
#define PUREPHONE_DBSERVICEAPI_HPP

#include "Service/Bus.hpp"
#include "Interface/SettingsRecord.hpp"

class DBServiceAPI {

public:
    static SettingsRecord SettingsGet(sys::Service* serv);
    static bool SettingsUpdate(sys::Service* serv,const SettingsRecord& rec);

};


#endif //PUREPHONE_DBSERVICEAPI_HPP
