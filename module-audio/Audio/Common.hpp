/*
 *  @file Common.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 24.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_COMMON_HPP
#define PUREPHONE_COMMON_HPP

using Position = float;
using Volume = float;
using Gain = float;

enum class RetCode{
    Success = 0,
    InvokedInIncorrectState,
    UnsupportedProfile,
    InvalidFormat,
    OperationCreateFailed,

};

#endif //PUREPHONE_COMMON_HPP
