handle all nostop pass


define target hookpost-remote
monitor reset 0
monitor halt
eval "monitor exec SetRTTAddr %p", &_SEGGER_RTT
b main
end