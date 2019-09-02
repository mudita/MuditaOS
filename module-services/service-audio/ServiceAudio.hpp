/*
 *  @file ServiceAudio.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_SERVICEAUDIO_HPP
#define PUREPHONE_SERVICEAUDIO_HPP

#include "Service/Service.hpp"
#include <functional>
#include "Audio/Audio.hpp"
#include "MessageType.hpp"

class ServiceAudio : public sys::Service {

public:

    ServiceAudio();

    ~ServiceAudio();

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl,sys::ResponseMessage* resp=nullptr) override;

    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final{
        LOG_FATAL("[ServiceAudio] PowerModeHandler: %d", static_cast<uint32_t>(mode));
        return sys::ReturnCodes::Success;
    }

    static const char *serviceName;

private:
    audio::Audio audio;
    std::function<uint32_t (audio::AudioEvents event)> audioCallback = nullptr;
};


#endif //PUREPHONE_SERVICEAUDIO_HPP
