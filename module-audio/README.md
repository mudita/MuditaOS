# module-audio
Repository contains audio subsystem of PurePhone system

# Introduction
PureOS's audio subsystem was heavily inspired by two fine pieces of software:
#### Linux audio subsystem
Implementation of module-audio was developed after going through [Linux Sound Subsystem Documentation](https://www.kernel.org/doc/html/latest/sound/index.html) and various other
documents describing audio Linux audio. These documents were mainly used as inspiration as requested PureOS audio functionality is much less sophisticated. Also due to 
hardware limitations (mainly processor speed and hardware audio codec features) some additional assumptions were made.
#### [PortAudio library](http://www.portaudio.com/)
Whole audio device layer which can be found in `module-bsp/bsp/audio` and concept of audio devices were based on this library.
Even API is quite similar to PortAudio's one. 

# Features
* Playback/Recording/Routing(voice calls) operations
* MP3/FLAC/WAV mono/stereo audio files supported
* Voice calls recording (currently only WAV encoder is supported)
* FLAC tags support
* ID3V1 & ID3V2 tags support via [atag](https://github.com/mandreyel/atag)
* Supported frame rates: 8/16/32/44,1/48/96kHz in case of using headphones/loudspeaker
* Supported frame rates: `TODO` in case of using bluetooth 
* External events capturing (audio jack insert/remove)
* Output volume and input gain control
* Play/Pause/Resume/Stop controls
* Fetching audio file's tags on request
* Support for different audio profiles
* 16kHz samplerate during voice calls
* Mute/unmute/speakerphone during voice calls
* OPTIONAL: encrypted voice calls (not implemented yet but quite possible to do)
* BT A2DP and headset profile (not implemented yet)
* Headset volume control
* Automatic low-power management
* CTIA/OMTP/3-pole jack support

# Assumptions & limitations
#### No support for dynamic re-sampling
Due to limited processor speed it wasn't possible to implement dynamic re-sampling on the fly hence 
hardware audio codec and & BT configuration need to be switched according to specified audio file format which is
currently being played.
#### There is no possibility to mix audio channels
Due to not having re-sampling ability, mixing channels which have different audio formats is considered to be impossible.There is not even a audio channels or
mixer implemented! This has even further implications as it won't possible for instance to play notification during voice call or song. 
#### 16bit stereo/mono PCM samples are used as internal audio samples format
This format was picked as it is used the most common. Additionally moving raw PCM samples from one point to another is the easiest method and the most error prone.
#### 16 bit audio files are only supported
Currently due to some limitations in hardware audio codec only 16-bit PCM samples are supported. It is possible to add support for 8/24 or even 32 bit samples. This is should be done
in audio device layer inside `module-bsp/bsp/audio`
#### There is no possibility to record and playback at the same time
This is self-explanatory. Whole switching logic is implemented inside `module-audio` 
#### When using Iphone's standard headphones only 'Enter' button is functional
IPhone headphones have very specific format of handling vol up/down buttons which unfortunately is not available. By design only `Enter` button is supported.

# How it works
### Audio devices
Audio devices that can be found in `module-bsp/bsp/audio` are generic abstraction over different kinds of output/input audio devices.
For instance: audio codecs, GSM audio and so on.The main purpose of using this abstraction is to have one unified interface (API) by which we
can control and manage audio devices. This way we can also very easily support different compile target (currently Linux & RT1051). It is only a matter of
implementing a new audio device.

Currently we have four implementations of audio devices
* MAX98090 audio codec (RT1051 target)
* GSM audio (RT1051 target)
* Linux audio (via PortAudio library)
* Linux audio cellular (also via PortAudio library)

Each audio device must conform to API interface which is specified in `AudioDevice` class in `module-bsp/bsp/audio/bsp_audio.hpp`

### Decoders
Decoders layer was developed in order to have unified API over vast range of audio decoders. Another very important part of decoders
are ability to fetch audio metadata. Different kinds of decoders support different metadata, for instance MP3 decoder supports ID3 tags and so on.
All of this implementation details are hidden to the higher layers. User receives metadata via unified structure describing metadata. See `struct Tags`.

Currently there are three implementations:
* FLAC decoder
* WAV decoder
* MP3 decoder

### Encoders
The reasons for developing encoders layer were the same as for decoders. Currently only supported encoder is WAV.

### Operations
Audio module functionality is made of 4 base operations/states:
* Idle
* Playback
* Recorder
* Router

All possible state are described in enum given below:
````
enum class Type {
    Idle,
    Playback,
    Recorder,
    Router
};
````

Each state supports various methods which can be invoked in any time:
````
int32_t Start(std::function<int32_t(AudioEvents event)> callback)

int32_t Stop()

int32_t Pause()

int32_t Resume()

int32_t SendEvent(const Event evt, const EventData *data=nullptr)

int32_t SetOutputVolume(float vol)

int32_t SetInputGain(float gain)

Position GetPosition()

Volume GetOutputVolume();

Gain GetInputGain()

State GetState()

const Profile *GetProfile()
````
Vast majority of them is self-explanatory except `SendEvent` method. This method is mainly used to send various external events to current state.
All available events that can be passed to this method are listed below:
````
enum class Event {
    HeadphonesPlugin,
    HeadphonesUnplug,
    BTHeadsetOn,
    BTHeadsetOff,
    BTA2DPOn,
    BTA2DPOff,
    StartCallRecording,
    StopCallRecording,
    CallMute,
    CallUnmute,
    CallSpeakerphoneOn,
    CallSpeakerphoneOff,
};
````
As you can see there are lots of them by not every event is supported in every state. For instance, `StartCallRecording` and `StopCallRecording` are not supported
in `Playback` operation and will be ignored upon receiving.

##### Idle
This is default state of audio subsystem. Audio subsystem always operates in this state when:
* there is no music being played
* nothing is recorded
* there is no routing activity (during voice calls)

Audio subsystem will also transition to this state upon several events:
* receiving `Stop` request no matter in what current state
* playback end 
* recording failure (for instance running out of disk space)
* any other failure during switching between different states. Audio will always fallback to idle state if there is any kind of error.

Also in this state audio subsystem tries to optimize power consumption by closing/removing all audio devices and limiting internal operations to minimum.

##### Playback
Playback operation is used when user wants to play audio file. File's extension is used to deduce which decoder to use. 

##### Recorder
Recorder operation is used when user wants to record external sound for example via internal microphone. As mentioned earlier for the time being only WAV encoder is supported.

##### Router
Router operation is used in connection with GSM modem and it provides means for establishing audio voice call. Under the hood router operation uses two audio devices simultaneously configured
as full-duplex(both Rx and Tx channels) and routes audio samples between them. Additionally when routing it is possible to sniff or store audio samples to external buffers/file system. 
This feature is currently mainly used to record voice-calls into the file.

### Audio profiles
There is one more very important element of this puzzle and it is concept of `Audio Profiles`. List of supported audio profiles is listed below:
````
enum class Type {
    // Profiles used only during call
    RoutingSpeakerphone,
    RoutingHeadset,
    RoutingBTHeadset,
    RoutingHeadphones,
    RoutingEarspeaker,

    // Recording profiles
            RecordingBuiltInMic,
    RecordingHeadset,
    RecordingBTHeadset,

    // Profiles used by music player
            PlaybackLoudspeaker,
    PlaybackHeadphones,
    PlaybackBTA2DP,

    // Profiles used by system sounds
            SystemSoundLoudspeaker,
    SystemSoundHeadphones,
    SystemSoundBTA2DP,

    Idle,

};
````

In contrast to `Operations` which are functional blocks of audio subsystem `AudioProfiles` serve different purpose. They exist to complement `Operations` as they main
job is to store audio configuration which then `Operations` use to configure audio devices. Each `Operation` supports its specific set of profiles which can be switched back and forth. To do so
mechanism od sending external events is used which was described earlier. For instance sending `HeadphonesPlugin` event when in `Playback` will result in switching from current profile
which can be for example `PlaybackLoudspeaker` to `PlaybackHeadphones` profile. Switching profile triggers some additional internal actions:
* current profile is replaced with new one
* current audio device is reloaded with the new parameters
* in case of any error system will fallback to `Idle` operation and report error

Example of profile structure is given below:
````
ProfilePlaybackHeadphones(std::function<int32_t()> callback, float volume) : Profile(
        "Playback Headphones", Type::PlaybackHeadphones,
        bsp::AudioDevice::Format{.sampleRate_Hz=0,
                .bitWidth=16,
                .flags=0,
                .outputVolume=volume,
                .inputGain=0,
                .inputPath=bsp::AudioDevice::InputPath::None,
                .outputPath=bsp::AudioDevice::OutputPath::Headphones
        },
        bsp::AudioDevice::Type::Audiocodec,
        callback) {}
````
Not every parameter is used, for example setting `inputGain` for playback does not have any sense so does `inputPath` hence they are omitted.
`callback` parameter was designed to be invoked asynchronously when audio subsystem updates profile's data. This way those changes can be also updated in database.  
**IMPORTANT:** For the time being profiles are not loaded and stored into database. This should be fixed.  
**IMPORTANT:** Callbacks mechanism is only experimental and should be considered as incomplete.


# Missing features
#### Bluetooth audio device support
#### Jack insert/remove events handling
#### Storing & loading profiles
#### Control profiles parameters


More missing features are listed in `module-audio/TODO` file.