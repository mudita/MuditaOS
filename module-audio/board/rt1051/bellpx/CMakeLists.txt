target_sources(${AUDIO_BOARD_LIBRARY}
    PRIVATE
        BellPxAudioDeviceFactory.cpp
        BellPxAudioCodec.cpp
    
    PUBLIC
        BellPxAudioCodec.hpp
        BellPxAudioDeviceFactory.hpp
)
