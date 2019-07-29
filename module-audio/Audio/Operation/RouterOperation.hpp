/*
 *  @file RouterOperation.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_ROUTEROPERATION_HPP
#define PUREPHONE_ROUTEROPERATION_HPP


#include <memory>
#include <optional>
#include <functional>

#include "Operation.hpp"

class RouterOperation : public Operation {
public:

    RouterOperation(const char *file);

    int32_t Start([[maybe_unused]] std::function<int32_t (uint32_t)> callback) override final;

    int32_t Stop() override final;

    int32_t Pause() override final;

    int32_t Resume() override final;

    int32_t SendEvent(const Event evt,const EventData* data=nullptr) override final {return 0;}

    int32_t SwitchProfile(const Profile::Type type)  override final;

    int32_t SetOutputVolume(float vol) override final;

    int32_t SetInputGain(float gain) override final;

    Position GetPosition() override final {return 0.0;}
};


#endif //PUREPHONE_ROUTEROPERATION_HPP
