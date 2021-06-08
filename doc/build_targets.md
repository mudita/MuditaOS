# Products

In one source tree we have two products:

* PurePhone
* BellHybrid

For simplicity, we have defined a few CMake targets that will create
all required images and prepare update packages.

# Architectures

We support two target architectures:

* Linux x86_64
* RT1051

Some build targets are enabled/disabled based on the selected target architecture.

# Targets


| Arch | Name | Alias | Description |
|------|------|-------|-------------|
|common| Pure | | Main target for PurePhone |
|common| Bell | | Main target for BellHybrid |
|common| PurePhone-disk-img  | PurePhone.img  | Disk image for PurePhone  |
|common| BellHybrid-disk-img | BellHybrid.img | Disk image for BellHybrid | 
|RT1051| PurePhone-StandaloneImage  | PurePhone-\<version\>-RT1051-package-standalone  | Creates image that can be `dd` or `pureflash` to the device|
|RT1051| PurePhone-UpdatePackage    | PurePhone-\<version\>-RT1051-Update.tar          | Creates Update package, that can be used by Mudita Center or an update script|
|RT1051| BellHybrid-StandaloneImage | BellHybrid-\<version\>-RT1051-package-standalone | Creates image that can be `dd` or `pureflash` to the device|
|RT1051| BellHybrid-UpdatePackage   | BellHybrid-\<version\>-RT1051-Update.tar         | Creates Update package, that can be used by Mudita Center or an update script|

