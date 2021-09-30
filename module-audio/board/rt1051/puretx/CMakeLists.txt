target_sources(${AUDIO_BOARD_LIBRARY}
    PRIVATE
        PureTxAudioCodec.cpp
        PureTxAudioDeviceFactory.cpp
        RT1051CellularAudio.cpp
    
    PUBLIC
        PureTxAudioCodec.hpp
        PureTxAudioDeviceFactory.hpp
        RT1051CellularAudio.hpp
)
