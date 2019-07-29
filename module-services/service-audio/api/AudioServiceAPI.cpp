/*
 *  @file AudioSerivceAPI.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "AudioServiceAPI.hpp"
#include "Service/Bus.hpp"
#include "../ServiceAudio.hpp"

using namespace audio;


RetCode AudioServiceAPI::PlaybackStart(sys::Service *serv, const std::string &fileName) {
    std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioPlaybackStart);
    msg->fileName = fileName;

    auto ret = sys::Bus::SendUnicast(msg,ServiceAudio::serviceName,serv,5000);
    return reinterpret_cast<AudioResponseMessage*>(ret.second.get())->retCode;
}

RetCode AudioServiceAPI::Stop(sys::Service *serv) {
    std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioStop);

    auto ret = sys::Bus::SendUnicast(msg,ServiceAudio::serviceName,serv,5000);
    return reinterpret_cast<AudioResponseMessage*>(ret.second.get())->retCode;
}

RetCode AudioServiceAPI::Pause(sys::Service *serv) {
    std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioPause);

    auto ret = sys::Bus::SendUnicast(msg,ServiceAudio::serviceName,serv,5000);
    return reinterpret_cast<AudioResponseMessage*>(ret.second.get())->retCode;
}

RetCode AudioServiceAPI::Resume(sys::Service *serv) {
    std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioResume);

    auto ret = sys::Bus::SendUnicast(msg,ServiceAudio::serviceName,serv,5000);
    return reinterpret_cast<AudioResponseMessage*>(ret.second.get())->retCode;
}

RetCode AudioServiceAPI::RecordingStart(sys::Service *serv, const std::string &fileName) {
    std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioRecorderStart);
    msg->fileName = fileName;

    auto ret = sys::Bus::SendUnicast(msg,ServiceAudio::serviceName,serv,5000);
    return reinterpret_cast<AudioResponseMessage*>(ret.second.get())->retCode;
}

RetCode AudioServiceAPI::SetInputGain(sys::Service *serv, const Gain gain) {
    std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioSetInputGain);
    msg->val = gain;

    auto ret = sys::Bus::SendUnicast(msg,ServiceAudio::serviceName,serv,5000);
    return reinterpret_cast<AudioResponseMessage*>(ret.second.get())->retCode;
}

RetCode AudioServiceAPI::SetOutputVolume(sys::Service *serv, const Volume vol) {
    std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioSetOutputVolume);
    msg->val = vol;

    auto ret = sys::Bus::SendUnicast(msg,ServiceAudio::serviceName,serv,5000);
    return reinterpret_cast<AudioResponseMessage*>(ret.second.get())->retCode;
}

std::optional<Tags> AudioServiceAPI::GetFileTags(sys::Service *serv, const std::string &fileName) {
    std::shared_ptr<AudioRequestMessage> msg = std::make_shared<AudioRequestMessage>(MessageType::AudioGetFileTags);
    msg->fileName = fileName;

    auto ret = sys::Bus::SendUnicast(msg,ServiceAudio::serviceName,serv,5000);
    if(reinterpret_cast<AudioResponseMessage*>(ret.second.get())->retCode != RetCode::Success){
        return {};
    }
    else{
        return reinterpret_cast<AudioResponseMessage*>(ret.second.get())->tags;
    }
}

