#include "AudioMessage.hpp"
#include "../ServiceAudio.hpp"

audio::Handle AudioRequestSync::SendSync(sys::Service *serv)
{
    auto ret = sys::Bus::SendUnicast(shared_from_this(), ServiceAudio::serviceName, serv, sys::defaultCmdTimeout);
    if (ret.first == sys::ReturnCodes::Success) {
        if (auto resp = std::dynamic_pointer_cast<AudioResponseMessage>(ret.second)) {
            return audio::Handle(resp->retCode, resp->token);
        }
    }
    return audio::Handle(audio::RetCode::Failed, audio::Token::MakeBadToken());
}

bool AudioRequestAsync::SendAsync(sys::Service *serv)
{
    return sys::Bus::SendUnicast(shared_from_this(), ServiceAudio::serviceName, serv);
}
