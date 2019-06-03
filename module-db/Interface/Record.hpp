
/*
 * @file Record.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_RECORD_HPP
#define PUREPHONE_RECORD_HPP

#include <memory>
#include "../Database/Database.hpp"

class Record {
public:

    Record();

    virtual ~Record();

    virtual bool Add();

    virtual bool Remove();

    virtual bool Update();



};


#endif //PUREPHONE_RECORD_HPP
