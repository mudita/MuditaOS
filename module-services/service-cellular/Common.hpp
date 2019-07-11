
/*
 * @file Common.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 11.07.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_COMMON_HPP
#define PUREPHONE_COMMON_HPP



enum class CallStates{
    Active, // 0 active: call in progress (setup was successful)
    Held,   // 1 held: call on hold
    Dialing,    // 2 dialing (MO call): number dialed
    Alerting,   // 3 alerting (MO call): number dialed and the called party is alerted
    Incoming,   // 4 incoming (MT call): incoming call, ringtone played (AT RING notification)
    Waiting     // 5 waiting (MT call): call waiting notification while another call is active (if call waiting feature enabled)
};

#endif //PUREPHONE_COMMON_HPP
