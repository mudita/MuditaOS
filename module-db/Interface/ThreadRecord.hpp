
/*
 * @file ThreadRecord.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_THREADRECORD_HPP
#define PUREPHONE_THREADRECORD_HPP

#include "Record.hpp"
#include <stdint.h>
#include "utf8/UTF8.hpp"
#include "../Common/Common.hpp"

class ThreadRecord : public Record<ThreadRecord>{
public:

    uint32_t dbID;
    uint32_t date;
    uint32_t msgCount;
    uint32_t read;
    uint32_t contactID;
    UTF8 snippet;
    uint32_t lastDir;
};


#endif //PUREPHONE_THREADRECORD_HPP
