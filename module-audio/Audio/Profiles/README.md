# Profile JSON file
This document aims to describe the profile file and how to tweak those values.

## First things first
The JSON files describing the profile parameters are used to configure the audio profile with proper input/output and filter settings. 
It is **NOT** recommended to change anything than the filter settings as it might make your device speechless.

According to the [MAX98090 datasheet](https://datasheets.maximintegrated.com/en/ds/MAX98090.pdf), it is possible to have 7-band-EQ, although we're 
using only 5-band-EQ right now. It can be easily changed via proper register setup. All filters are realized using the [biquad filters](https://en.wikipedia.org/wiki/Digital_biquad_filter) (second order IIR filter).
In order to prevent audio issues, when no file is present or the JSON structure is damaged, the fallback config is being loaded.

##File structure

| Field             | Value type | Description                                                 | Valid range           |
|-------------------|------------|-------------------------------------------------------------|-----------------------|
| samplerate        | integer    | Defines the sample rate of the profile                      | 8000 - 96000 Hz       |
| bitWidth          | integer    | Defines the bit width of the audio stream                   | 8 b - 32 b            | 
| flags             | integer    | Defines the input/output channels (see codec.hpp)           | 0 - 4                 |
| outputVolume      | float      | Defines the output volume for the particular profile        | 0 - 10                |
| inputGain         | float      | Defines the input gain for the particular profile           | 0 - 10                |
| playbackPathGain  | integer    | Defines the gain of MAX98090 playback path                  | 0 - 3 (0 dB - 18 dB)  |
| playbackPathAtten | integer    | Defines the attenuation of MAX98090 playback path           | 0 - 15 (0 dB - 15 dB) |
| inputPath         | integer    | Defines the audio path for profile's input (see codec.hpp)  | 0 - 2                 |
| outputPath        | integer    | Defines the audio path for profile's output (see codec.hpp) | 0 - 3                 |
| filterParams      | array      | Array of filter parameters (one per band)                   | See table below       |

Parameter _playbackPathGain_ is adjusted in 6dB increments; _playbackPathAtten_ is adjusted in 1dB increments, as described
in Table 49 (p. 120) in MAX98090 datasheet.

### Filter parameters array

The filter parameters array consists of a one JSON struct per filter in the codec. It means that adding next element in the array
does not give any effect, but removing one of them will lead to wrong audio configuration! If you want to disable unused filter, select
appropriate filter type - `None`.

### Array element structure

| Field      | Value type | Description                                                | Valid range    |
|------------|------------|------------------------------------------------------------|----------------|
| filterType | string     | Defines the type of the filter                             | See list below |
| frequency  | float      | Defines the cutoff frequency for the filter                | \> 0 Hz        | 
| samplerate | integer    | Defines the samplerate used to calculate the filter coeffs | \> 0 Hz        |
| Q          | float      | Defines Q factor for the filter                            | 0.1 - 10       |
| gain       | float      | Defines gain for shelf-like filters                        | \> 0           |

Filter type can be selected from following types:

* `LowPass`  - simple low pass filter
* `HighPass` - simple high pass filter
* `HighShelf` - shelving high pass filter
* `LowShelf` - shelving low pass filter
* `Notch` - notch filter
* `Parametric` - parametric filter
* `Flat` - flat transfer function filter
* `None` - filter is disabled

