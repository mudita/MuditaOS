# module-audio
Repository contains audio subsystem of PurePhone system

# Introduction
PureOS's audio subsystem was heavily inspired by two fine pieces of software:
#### Linux audio subsystem
Implementation of module-audio was developed after going through [Linux Sound Subsystem Documentation](https://www.kernel.org/doc/html/latest/sound/index.html) and various other documents describing audio Linux audio. These documents were mainly used as inspiration as requested PureOS audio functionality is much less sophisticated. Also due to hardware limitations (mainly processor speed and hardware audio codec features) some additional assumptions were made.
#### Paths

 * [/module-bsp/bsp/audio](/module-bsp/bsp/audio) - audio device interface and switching logic
 * [/module-bsp/board/linux/audio](/module-bsp/board/linux/audio) - Linux audio devices
 * [/module-bsp/board/rt1051/bsp/audio](/module-bsp/board/rt1051/bsp/audio) - RT1051 audio devices
 * [/module-audio/Audio](/module-audio/Audio) - audio module

#### [PortAudio library](http://www.portaudio.com/)
Audio device layer (which can be found in [audio paths](#paths)). concept of audio devices and audio module API were based on PortAudio library.

# Features
* Playback/Recording/Routing(voice calls) operations
* MP3/FLAC/WAV mono/stereo audio files supported
* Voice calls recording (currently only WAV encoder is supported)
* MP3/FLAC/WAV tags support
* ID3V1 & ID3V2 tags support via [taglib](https://github.com/taglib/taglib)
* Supported frame rates: 8/16/32/44,1/48/96kHz in case of using headphones/loudspeaker
* Supported frame rates: `TODO` in case of using bluetooth 
* External events capturing (audio jack insert/remove)
* Output volume and input gain control
* Play/Pause/Resume/Stop controls
* Fetching audio file's tags on request
* Support for different audio profiles
* 16kHz samplerate during voice calls
* Mute/unmute/speakerphone during voice calls
* BT A2DP and headset profile (not implemented yet)
* Headset volume control
* Automatic low-power management
* CTIA/OMTP/3-pole jack support

# Assumptions & limitations
#### No support for dynamic re-sampling
Due to limited processor speed it wasn't possible to implement dynamic re-sampling on the fly hence hardware audio codec and & BT configuration need to be switched according to specified audio file format which is currently being played.
#### There is no possibility to mix audio channels
Due to lack of re-sampling ability, channel abstraction does not exist thus it is not possible to play notification during voice call or media playback. 
#### 16bit stereo/mono PCM samples are used as internal audio samples format
This format was picked as it is commonly used. Additionally moving raw PCM samples from one point to another is the easiest method and the most error resilient.
#### 16 bit audio files are only supported
Currently due to hardware limitations audio codec only 16-bit PCM samples are supported. It is possible to add support for 8/24 or even 32 bit samples. This should be done in audio device layer inside `module-bsp/bsp/audio`
#### There is no possibility to record and playback at the same time
This is self-explanatory. Whole switching logic is implemented inside `module-audio` 
#### When using Iphone's standard headphones only 'Enter' button is functional
IPhone headphones have very specific format of handling vol up/down buttons which unfortunately is not available. By design only `Enter` button is supported.

# How it works
### Audio devices
Audio devices that can be found in [audio paths](#paths) are generic abstraction over different kinds of output/input audio devices. For instance: audio codecs, GSM audio and so on.The main purpose of using this abstraction is to have one unified interface (API) for controlling and managing audio devices. This way we can also easily support different compile targets (currently Linux & RT1051). It is only a matter of implementing a new audio device.

Currently we have four implementations of audio devices
* MAX98090 audio codec (RT1051 target)
* GSM audio (RT1051 target)
* Linux audio (via PortAudio library)
* Linux audio cellular (also via PortAudio library)

Each audio device must conform to API interface which is specified in `bsp::AudioDevice` class in [bsp_audio](/module-bsp/bsp/audio/bsp_audio.hpp)

### Decoders
Decoders layer was developed in order to have unified API over vast range of audio decoders. Another very important part of decoders are ability to fetch audio metadata. Different kinds of decoders support different metadata, for instance MP3 decoder supports ID3 tags and so on. All of this implementation details are hidden to the higher layers. User receives metadata via unified structure describing metadata. See `struct Tags`.

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

Possible states are enumered in `audio::Audio::State`. Each state supports various methods (defined in interface class audio::Operation) which can be invoked at any time. It is possible to send external events to current `Operation` and this function is realised through `SendEvent` method. Available events that can be passed to this method are enumrated in `Operation::Event`

Not all events are supported in every state. For instance, `StartCallRecording` and `StopCallRecording` are not supported in `Playback` operation and will be ignored upon receiving.

##### Idle
This is default state of audio subsystem. Audio subsystem always operates in this state when none of below is active:
* media playback
* recording
* routing (voice calls)

Audio subsystem will also transition to this state upon following events:
* receiving `Stop` request no matter in what current state
* playback end 
* recording failure (for instance running out of disk space)
* any other failure during switching between different states or internal error 

In this state audio subsystem tries to optimize power consumption by closing/removing all audio devices and limiting internal operations to minimum.

##### Playback
Playback operation is used when user wants to play audio file. File's extension is used to deduce which decoder to use. 

##### Recorder
Recorder operation is used when user wants to record external sound for example via internal microphone. As mentioned earlier for the time being only WAV encoder is supported.

##### Router
Router operation is used in connection with GSM modem and it provides means for establishing audio voice call. Under the hood router operation uses two audio devices simultaneously configured as full-duplex(both Rx and Tx channels) and routes audio samples between them. Additionally when routing it is possible to sniff or store audio samples to external buffers/file system. This feature is currently mainly used to record voice-calls into the file.

### Audio profiles
In order to store `Operation` configuration a concept of `Audio Profile` has been introduced. List of supported audio profiles is enumerated in `Profile::Type`  

`Audio Profiles` exist to complement `Operations`. They store audio configuration which then `Operations` use to configure audio devices. Each `Operation` supports its specific set of `Audio Profiles` which can be switched back and forth. To do so mechanism of sending external events is used which was described earlier. For instance sending `HeadphonesPlugin` event when in `Playback` will result in switching from current profile i.e.`PlaybackLoudspeaker` to `PlaybackHeadphones` profile. Switching profile triggers some additional internal actions:
* current profile is replaced with new one
* current audio device is reloaded with the new parameters
* in case of any error system will fallback to `Idle` operation and report error

Profiles configurations can be found in directory: [Profiles](./Audio/Profiles)  
`Operations` may not implement support for all possible `Profile` parameters i.e. `inputGain` and `inputPath` will be ignored in playback `Operation`.

~~**IMPORTANT:** For the time being profiles are not loaded and stored into database. This should be fixed.~~  
**NOTE:** Currently some of the profiles are configurable via json files located [here](/image/user/data/equalizer). The json format explanation can be found [here](./Audio/Profiles/README.md).

**IMPORTANT:** Callbacks mechanism is only experimental and should be considered as incomplete.

# Audio class
Audio class [Audio class](./Audio/Audio.hpp) is main interface to audio subsystem. It is exclusively used by [audio-service](/module-services/service-audio) Audio class stores internally current `Operation`, `Profile`, `State`, async callback, db callback etc. 

`AsyncCallback` holds a wrapper for function that is invoked asynchronously when audio subsystem event occurs. It can be used for signalling user of audio subsystem about audio events. Possible events are listen in `audio::PlaybackEventType`  

Audio class APIs may change `State` of audio subsystem.  

User should be aware of following constraints:
* It is possible to invoke `Start` from any state
* It is possible to invoke `Stop` from any state
* `Pause` can be invoked only when **not** in `Idle` state
* `Resume` can be invoked only **after** successful `Pause` request.

Failing to adhere will end in returning appropriate error.  


# Missing features
#### Bluetooth audio device support
Currently support for BT audio devices is marginal. There are some code parts related to BT but they absolutely cannot be treated as target implementation. Adding BT support in my opinion should be split into several steps:
* Adding BT A2DP audio device into `module-bsp/bsp/audio`
* Adding necessary profiles configuration (`PlaybackBTA2DP` and `SystemSoundBTA2DP`)
* There should be no further problems with BT audio device if it fully implements necessary API and its behaviour under different actions is correct
* For examples of audio device implementation please check existing ones

#### Jack insert/remove events handling
Templates for all necessary profiles are already implemented. There should be verified and double-checked, though. Jack insert/remove events are also correctly propagated into audio subsystem. What is missing is actual handling of jack detection circuit on the low-level. There is also no communication between audio subsystem and event system.

#### Storing & loading profiles
As described earlier dedicated async callback were added into each profile. It is to be considered if this mechanism is sufficient enough or should be reimplemented. Currently profiles configuration is not sustained and loaded upon start. It should be stored in DB.  

Some missing features are also described in [TODO](./TODO)
