/*
 *  @file Playback.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_PLAYBACK_HPP
#define PUREPHONE_PLAYBACK_HPP

#include <memory>
#include <optional>
#include <functional>

#include "Operation.hpp"

class decoder;


class Playback : public Operation {
public:

    Playback(const char *file, const Profile* profile);
    virtual ~Playback();

    int32_t Start(std::function<int32_t (uint32_t)> callback) override final;

    int32_t Stop() override final;

    int32_t Pause() override final;

    int32_t Resume() override final;

    int32_t SwitchProfile(const Profile* prof)  override final;

    Position GetPosition() override final;

private:
    std::unique_ptr<decoder> dec;
};


#endif //PUREPHONE_PLAYBACK_HPP
