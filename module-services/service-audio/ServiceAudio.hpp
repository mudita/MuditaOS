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

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl) override;

    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes WakeUpHandler() override;

    sys::ReturnCodes SleepHandler() override;

    static const char *serviceName;

private:
    Audio audio;
    std::function<uint32_t (AudioEvents event)> audioCallback = nullptr;
};


#endif //PUREPHONE_SERVICEAUDIO_HPP
