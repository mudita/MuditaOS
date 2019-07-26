target remote :2331
monitor reset
set mem inaccessible-by-default off
monitor clrbp
monitor halt
load
b main
c
