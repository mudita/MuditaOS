/*
 *  @file IdleOperation.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "IdleOperation.hpp"


IdleOperation::IdleOperation([[maybe_unused]] const char *file) {
    //TODO: M.P switch off audio PLL here
    isInitialized = true;
}

IdleOperation::~IdleOperation() {
    //TODO: M.P switch on audio PLL here
}