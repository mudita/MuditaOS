# PurePhone repository

## First time repo checkout with submodules
`git submodule update --init --recursive`

## Repo update with submodules
`git pull --recurse-submodules`

## Documentation:

* [doc/config](./doc/config.linux.md)
* [doc/bluetooth](./doc/bluetooth.md)
    * [./module-bsp/bsp/bluetooth](./module-bsp/bsp/bluetooth/)
    * [./module-bluetooth/](./module-bluetooth/README.md)
* [module-lwip](./module-lwip/README.md)
* [doc/database](./doc/database_v2.md)
* [emulator keyboard bindings](./doc/host_keyboard_bindings.md)

## Linux Bluetooth device

Bluetooth service by default initializes bluetooth device. 
On linux it's tellit developer kit plugged via USB-CDC. To properly
inteface with bluetooth telit device (and not have conflicts in names)
please add following script as: `/etc/udev/rules.d/99-purephone.rules` on linux.

```
ACTION=="add", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", ATTRS{serial}=="AL02ZDVH", GROUP="dialout", SYMLINK+="telit"
ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", MODE="666", GROUP="dialout"
```

This will enable you to use Bluetooth telit board as `/dev/telit` which 
is expected by code.
To access devices without root priviledges add yourselve to dialout group.
`sudo usermod -a -G dialout $USER`

For changes to take place run:
`sudo udevadm control --reload-rules && sudo udevadm trigger`
