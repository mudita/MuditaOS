/*
 *  @file Audio.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 22.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "Audio.hpp"

#include "Operation/Operation.hpp"


Audio::Audio():currentOperation(){

    auto ret  = Operation::Create(Operation::Type::Idle,"");
    if(ret){
        currentOperation = std::move(ret.value());
    }

}

