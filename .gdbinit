handle SIGUSR1 noprint nostop

define hookpost-load
eval "monitor exec SetRTTAddr %p", &_SEGGER_RTT
b main
b ResetISR
b HardFault_Handler
end

# Only used when connecting to the running target, i.e without uploading the new image
define target hookpost-remote
eval "monitor exec SetRTTAddr %p", &_SEGGER_RTT
end
