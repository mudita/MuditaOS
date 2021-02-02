# Charger cutoff due to temperature

In order to prevent fast battery cell degradation, charging action needs to be prohibited in specific temperature range. For given cell valid charging range is 0-45 Cdeg. 

Current implementation cuts off charging when cell temperature is outside this range. Mechanism is interrupt-driven. In interrupt handler temperature measurement is sampled and appropriate interrupt range is set. The same action is done at the time of initialization. This way no cyclic sampling of the temperature has to be done. Algorithm could be described by following graph:

![](charger_temperature_cutoff.svg "Charging cutoff")

Additional 1 Cdeg hysteresis was introduced to prevent rapid changes in charging state.