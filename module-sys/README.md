# module-sys
Repository contains core sys functionality of PurePhone system (service abstraction and communication between them)

# How to use it

module-sys is made of four main parts which are thoroughly described below:

## System manager

System manager is responsible for:
* [resolving services dependencies at system startup](./SystemManager/doc/ServicesSynchronization.md)
* [system shutdown sequence](./SystemManager/doc/SystemCloseSequence.md)

## Services

In order to create a new custom service, you have to inherit from the base Service class.

**WARNING:** Services are not started at the moment their constructor is called. This essentially means that `sys::Bus` and every functionality based on bus is not available yet.
The first moment when you are capable to call `sys::Bus` is `InitHandler()` of application. This is the place where your service initialization code should happen.

Then you have to implement several virtual methods which are listed below:

#### `ReturnCodes InitHandler()`

This handler is invoked upon creation of the service. It is very important to return proper return code specified in:

```
enum class ReturnCodes{
    Success,
    Failure,
    Timeout,
    ServiceDoesntExist,
    Unresolved
};
```

#### `ReturnCodes DeinitHandler()`  
This handler is invoked upon destruction of the service. The same rules about return codes apply here.
#### `ReturnCodes SwitchPowerModeHandler(const ServicePowerMode mode)`  
This handler is invoked when there is request for switching specified service's power mode. Available power modes which
service should support are specified below:
```
enum class ServicePowerMode{
    Active,
    SuspendToRAM,
    SuspendToNVM
}; 
```

Important: Currently there is no distinction between `SuspendToRAM` and `SuspendsToNVM`. These two cases should be handled the same.  Additionally, only `SuspendToNVM` can be received by service.

#### `MessagePointer DataReceivedHandler(DataMessage* msg, ResponseMessage* resp)`  
This is the main handler which should handle incoming messages.  
If `DataMessage* msg` is not nullptr then it contains valid message which was sent by using blocking Bus API.  
If `ResponseMessage* resp` is not nullptr then it contains valid message which was sent by using non-blocking Bus API.

This handler MUST return valid `MessagePointer`. In practice each service will return custom ResponseMessage which is created
by inheriting from base `sys::ResponseMessage` class.  

Please check existing services for more examples of use    
Please check "Caveats & good practices" chapter for more info   

## Timers

Timers are system feature and is Service/Application thread safe. Timer callback life expectancy should be as short as possible for system to work smoothly (as with any other message call)

**WARNING:** Do not use FreeRTOS timers, without utmost care it can cause data races and obstruct sys::Timres and system in generall.

### System Timers

System has basic coarse sys::Timer capability. There are two ways to handle actions on timer:
* create a timer via TimerFactory 
* assign callback to Timer with method: `void sys::timer::SystemTimer::connect(timer::TimerCallback &&newCallback)`
* start the timer

example:
``` c++
th = sys::TimerFactory::createPeriodicTimer(
    this, "my  periodic timer", std::chrono::milliseconds(1000), [](sys::Timer &t) {
        LOG_INFO("timers are avesome, periodic timer is active: %d", t.isActive());
    });
th.start();
```

**NOTE:** System timers are RAII. These are automatically destructed when their handles are removed!
**NOTE:** We do not have real-time system timers. It's possible to implement these, but there is no good mechanism to actually promote thread to be the first to execute in the system.

### GUI Timers

* Build on top of `SystemTimer` is `GuiTimer` are meant as a connector between System <=> GUI.

### Hints
* Each service can have any amount of timers.
* For timer to work one have to first start it. Otherwise, timer is created and not started.
* After creating `GuiTimer` one can `connect` it to `gui::Item` related element with `Application::connect(GuiTimer &&, gui::Item*)` so that timer life-cycle would be same as Item referenced

## Bus

**WARNING:** Please mind that safe usage of the bus is available after `InitHandler()` was called

Bus subsystem was developed in order to allow cross-services communication.
Bus consists of four main methods:
#### `bool SendUnicast(std::shared_ptr<Message> msg,const std::string& service,Service* s)`  
Non-blocking variant of sending messages to specified service.  
`Service* s` is pointer to API caller.
#### `SendResult SendUnicast(std::shared_ptr<Message> msg,const std::string& service,Service* s,uint32_t timeout)`  
Blocking variant of sending messages to specified service. `timeout` is in ms. `SendResult` is std::pair which consist return code and message hence
it is required to first check for return code before using message field. In case of timeout return code will be set to `Timeout`    
`Service* s` is pointer to API caller.
#### `void SendMulticast(std::shared_ptr<Message> msg,BusChannels channel,Service* s)`  
Send message to specified channel. Every service which is registered to this channel will receive this message.  
`Service* s` is pointer to API caller.
#### `void SendBroadcast(std::shared_ptr<Message> msg,Service* s)`
Send message to all active service except the caller.  
`Service* s` is pointer to API caller.


## Workers
M.P: This section is incomplete mainly due to not having enough info about implementation. 

As far as I know workers were created as abstraction over FreeRTOS threads and are designed to tightly cooperate with services.
By design their lifecycle is controlled by services hence this relationship is a little similar to process(service)-thread(worker) except
that workers don't share memory/resources with parent service. They are also separate units of processing as they don't know anything about each other.

From my understanding they are mostly used as a mean of unloading services from doing cpu-intensive work which could block service's `DataReceivedHandler` for too long.
For examples of using them please check application code as it seems that's where they are used the most.

# Power management
MuditaOS [power management](./SystemManager/doc/PowerManagement.md) was designed and developed based on Linux Power Management subsystem.
There are three main assumptions:
* It is responsibility of each service to properly support power mode switching
* Each bsp package from module-bsp should use smart drivers when interfacing with hardware
* Each service should manage its software & hardware resources 

Most information about design and implementation can be found in [AND_0011_PowerManager](https://docs.google.com/document/d/1G1HUFEPGblu3_VDrDdwF1nVqKwMz6sz1nGOgMEmhJgs/edit#heading=h.gm0is2hpfho6).

Additionally, the current implementation of PowerManager(it should be considered as the first iteration of development, and absolutely it cannot be treated as a final solution) is very simple, but it proved to be working, and it fulfilled current requirements.
For the time being PowerManager class exposes two methods which are internally used by SystemManager:
#### `int32_t Switch(const Mode mode)`
This method allows for switching CPU to different modes which are listed below:
```
enum class Mode{
    FullSpeed,
    LowPowerRun,
    LowPowerIdle,
    Suspend

};
```
In current implementation only `FullSpeed` and `LowPowerIdle` modes are used. It is worth to note that `LowPowerIdle` is heavy customized
and it absolutely doesn't correspond to low power idle mode from RT1051's data sheet. Main differences are:
* All internal RAM controller banks are preserved
* External SDRAM controller is on but reconfigured to 12MHz clock in order to limit current consumption
* TODO: For now internal uC DCDC is NOT switched to low power mode. This should be fixed as it limits current consumption by over 2mA
* Core is switched to internal 24MHz oscillator, all available peripherals are turned off or switched to 24/12Mhz clock and external oscillator is turned off.

Actual code is implemented in `module-bsp/board/rt1051/bsp/lpm/RT1051LPM.cpp` and `module-bsp/board/rt1051/common/clock_config.cpp`.

The research was done regarding usage of the `Suspend` state, and it resulted in several conclusions:
* It is not possible to use it in MuditaOS
This is mostly due to software design where app code is executed from external SDRAM. It is almost impossible in current state of the system to
switch to suspend state and gracefully return to normal state when code is placed into SDRAM.
* It is not necessary to use `Suspend` state in order to fulfill business requirements (5mA in aeroplane mode)  
By clever use of SDRAM clock scaling and custom LowPowerIdle mode we were able to achieve even lower current consumption(2,08mA) which is more than
enough.

Based on above use of `Suspend` mode is not necessary and was dropped.

`LowPowerRun` mode is only listed for convenience as it is not even implemented.

#### `int32_t PowerOff()`
This method is used to turn off power supply to system. It is invoked by SystemManager after successful system close. 

### Features missing or to be considered:
#### Dynamic clock frequency scaling during normal operation 
This is optional feature but by implementing it we will be able to limit current consumption during normal operation. 

#### More sophisticated mechanism of entering and exiting from low power mode
Currently whole mechanism of switching services into low power mode is very simple. There are no additional checks being made and there is no
possibility to disable auto-lock feature and so on. Another thing is that most of the low-power logic is placed into ApplicationManager which unfortunately is
bad design choice. It is to be considered if current solution will be sufficient or not. It is very possible that more advanced mechanism of communication between
PowerManager and system logic will have to be designed and developed.

#### Core voltage in low power mode
Currently core voltage during operating in low power mode is set to 0.9V. This results in 2,08mA current consumption. This can be lowered even further by lowering it to 0.8V ( resulting in current consumed at around 1.86mA).
As far as I know setting core voltage to 0.8V is considered to be unstable, but it is worth trying/testing.  

Core voltage is set in LPM_EnterLowPowerIdle function which can be found in module-bsp/board/rt1051/common/clock_config.cpp.
For more info please check RT1051 Reference Manual, Chapter 18 "DCDC Converter" and DCDC Register 3.


# Caveats & good practices #

#### DataReceivedHandler should not block for long period of time.  
It is okay to do some minor blocking tasks but blocking for longer periods will make whole system unpredictable or behave
in very uncontrolled manner. It is perfectly fine to use `Bus::SendUnicast` blocking API in the handler.

#### Watch out for async incoming messages  
Some time ago second parameter was added to DataReceivedHandler 
`DataReceivedHandler(DataMessage* msg,ResponseMessage* resp)`
This addition was dictated by need of using non-blocking variant of `Bus::SendUnicast` which then can trigger receiving async
response. In this case second parameter is not nullptr used and should contain valid `ResponseMessage`, otherwise 
`ResponseMessage* resp` will be set to nullptr. Additionally, by design there won't be the situation where both `DataMessage* msg`
and `ResponseMessage* resp` are not nullptr. Users of services should be aware of that and always check if params are valid 
before using them.




