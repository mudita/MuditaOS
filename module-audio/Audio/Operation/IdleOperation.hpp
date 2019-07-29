/*
 *  @file IdleOperation.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_IDLEOPERATION_HPP
#define PUREPHONE_IDLEOPERATION_HPP

#include <memory>
#include <optional>
#include <functional>

#include "Operation.hpp"

class IdleOperation : public Operation {
public:

    IdleOperation(const char *file);
    ~IdleOperation();

    int32_t Start([[maybe_unused]] std::function<int32_t (uint32_t)> callback) override final {return 0;}

    int32_t Stop() override final {return 0;}

    int32_t Pause() override final {return 0;}

    int32_t Resume() override final {return 0;}

    int32_t SendEvent(const Event evt,const EventData* data=nullptr) override final {return 0;}

    int32_t SwitchProfile(const Profile::Type type)  override final {return 0;}

    int32_t SetOutputVolume(float vol) override final;

    int32_t SetInputGain(float gain) override final;

    Position GetPosition() override final {return 0.0;}
};


#endif //PUREPHONE_IDLEOPERATION_HPP
