/*
 *  @file StateMachine.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_STATEMACHINE_HPP
#define PUREPHONE_STATEMACHINE_HPP


#include <memory>

class StateMachine {
public:




protected:
    void ExternalEvent(uint8_t newState, const void* pData = nullptr);
    void InternalEvent(uint8_t newState, const void* pData = nullptr);

};


#endif //PUREPHONE_STATEMACHINE_HPP
