How to add new command handling in bluetooth
============================================

**remember**
Blutetooth Worker command handling uses [double dispatch](https://www.wikiwand.com/en/Double_dispatch) (if you are not familiar with the technique, see: [double dispatch example](https://dzone.com/articles/double-dispatch-in-c))

# To add a new event:

1. In `event/Events.hpp`
    - Add new event
    - remember to
        - inherit from base event
        - add override on `dispatch` function
    - Events may have const data inside, in such case please:
        - set const data in constructor
        - set data as const in structure
        - do not add needless getter/setter
1. In `AbstractController.hpp`:
    - Add new event to forward declaration
    - Add new event handler in the class below
2. In `WorkerController.{hpp,cpp}`
    - Add new event handler to worker controller implementation
    - Add event handling in the desired state
    - When adding new Handler or Event in the sml state machine please:
        - define structure per each event handler
        - **this way we have it's name in the logs wich** is super helpful
3. In `test` catalog
    - add unit tests regarding the state added
    - **please add tests for both: success and failure** scenarions, not only the green path

# Bluetooth command handling state diagram

# To generate state machine diagram

`make bluetooth_uml` then `./bluetooth_uml > bluetooth_uml.puml` and generate diagram via plantuml from that
