
/*
 * @file Common.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_COMMON_HPP
#define PUREPHONE_COMMON_HPP

/**
 * Types of sms message
 */
enum class SMSType {
    DRAFT = 0x01,
    FAILED = 0x02,
    INBOX = 0x04,
    OUTBOX = 0x08,
    QUEUED = 0x10,
    ALL = 0xFF
};

#endif //PUREPHONE_COMMON_HPP
