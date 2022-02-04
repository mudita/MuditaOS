# Profile JSON file
This document aims to describe the profile file and how to tweak those values.

## First things first
The JSON files describing the profile parameters are used to configure the audio profile with proper input/output and filter settings. 
It is **NOT** recommended to change anything than the filter settings as it might make your device speechless.

According to the [MAX98090 datasheet](https://datasheets.maximintegrated.com/en/ds/MAX98090.pdf), it is possible to have 7-band-EQ, although we're 
using only 5-band-EQ right now. It can be easily changed via proper register setup. All filters are realized using the [biquad filters](https://en.wikipedia.org/wiki/Digital_biquad_filter) (second order IIR filter).
In order to prevent audio issues, when no file is present or the JSON structure is damaged, the fallback config is being loaded.

##File structure

| Field        | Value type | Description                                                 |
|--------------|------------|-------------------------------------------------------------|
| samplerate   | integer    | Defines the sample rate of the profile                      |
| bitWidth     | integer    | Defines the bit width of the audio stream                   |
| flags        | integer    | Defines the input/output channels (see codec.hpp)           |
| outputVolume | float      | Defines the output volume for the particular profile        |
| inputGain    | float      | Defines the input gain for the particular profile           |
| inputPath    | integer    | Defines the audio path for profile's input (see codec.hpp)  |
| outputPath   | integer    | Defines the audio path for profile's output (see codec.hpp) |
| filterParams | array      | Array of filter parameters (one per band)                   |

### Filter parameters array

The filter parameters array consists of a one JSON struct per filter in the codec. It means that adding next element in the array
does not give any effect, but removing one of them will lead to wrong audio configuration! If you want to disable unused filter, select
appropriate filter type - `None`.

### Array element structure

| Field      | Value type | Description                                                |
|------------|------------|------------------------------------------------------------|
| filterType | string     | Defines the type of the filter                             |
| frequency  | float      | Defines the cutoff frequency for the filter                |
| samplerate | integer    | Defines the samplerate used to calculate the filter coeffs |
| Q          | float      | Defines Q-factor for the filter                            |
| gain       | float      | Defines gain for shelf-like filters                        |

Filter type can be selected from following types:

* `LowPass`  - simple low pass filter
* `HighPass` - simple high pass filter
* `HighShelf` - shelving high pass filter
* `LowShelf` - shelving low pass filter
* `Notch` - notch filter
* `Parametric` - parametric filter
* `Flat` - flat transfer function filter
* `None` - filter is disabled

