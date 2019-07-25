/*
 *  @file PlaybackOperation.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_PLAYBACKOPERATION_HPP
#define PUREPHONE_PLAYBACKOPERATION_HPP

#include <memory>
#include <optional>
#include <functional>

#include "Operation.hpp"

class decoder;

namespace bsp{
    class AudioDevice;
}


class PlaybackOperation : public Operation {
public:

    PlaybackOperation(const char *file);

    int32_t Start(std::function<int32_t (uint32_t)> callback) override final;

    int32_t Stop() override final;

    int32_t Pause() override final;

    int32_t Resume() override final;

    int32_t SwitchProfile(const Profile::Type type)  override final;

    Position GetPosition() override final;

private:
    std::unique_ptr<decoder> dec;
    std::unique_ptr<bsp::AudioDevice> audioDevice;
};


#endif //PUREPHONE_PLAYBACKOPERATION_HPP
