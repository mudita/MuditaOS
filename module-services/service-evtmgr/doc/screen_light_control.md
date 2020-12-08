# Screen frontlight control

## Modes of operation

Screen frontlight can be controlled in Manual and Standalone mode.

### Manual mode

Manual mode refers to constant level of the screen brightness passed to
screen_light_control module via messages in field `sevm::screen_light_control::Parameters::manualModeBrightness` .

### Automatic mode

In the automatic mode screen light is controlled using data from the ambient light sensor. This data is 
then processed with brightness function mapping to obtain particular level of screen lighting. Control algoritm contains ramp, hysteresis and gamma correction calculation.

![](light_control_algorithm.svg "light control algorithm")

## Brightness level function
Brightness level [%] in relation to ambient light luminance level [Lux] is passed as 
a vector of points wchich are automatically interpolated with linear function. `sevm::screen_light_control::Parameters::functionPoints` field is used for parametrization. These
points have to be in ascending order. Example:

```
sevm::screen_light_control::BrightnessFunction({{50.0f, 30.0f}, {150.0f, 80.0f}, {400.0f, 80.0f}, {700.0f, 40.0f}});
```
Which could be translated as follows:

![](light_control_function.svg "light control function example")

NOTE: Values before first point and after last point are set to constant level of brightness described by this points. Values in between are interpolated.

## Ramp
Values of screen brightness are udpated in faster control loop with ramping to acheive smooth change of light. Ramp value could be parametrized in `sevm::screen_light_control::Parameters::rampTimeMS` and refers to 0-100% change of brightness in given number of miliseconds.

## Hysteresis

Hysteresis value is adjustable via API using `sevm::screen_light_control::Parameters::brightnessHysteresis`.

![](light_control_hysteresis.svg "Hysteresis in ramp")

## Gamma correction
Gamma correction is algorith to adjust change of the light intesivity in the way that it is perveied as linear by human eye.

y(x) = scale*(x/scale) <sup>&gamma;</sup>

Where &gamma; is parametrizable correction factor in `sevm::screen_light_control::Parameters::gammaFactor`.

![](gamma_correction.svg "Human eye sensitivity")

## Message API

`sevm::ScreenLightControlMessage` is used to control the module. It takes action and data structure as parameters. Set of action is described in `sevm::screen_light_control::Action`.