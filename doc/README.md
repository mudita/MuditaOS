# MuditaOS Documentation

This folder contains the documentation of the MuditaOS.

## How to generate documentation using Doxygen

Fully detailed documentation can be built locally using [Doxygen](https://www.doxygen.nl/index.html).

You can build code documentation with the following command:
```sh
make doc
```
If you prefer to build documentation always with `all` target instead, configure `cmake`
with `BUILD_DOC_WITH_ALL` option set to `ON`, e.g.:
```sh
cmake -DBUILD_DOC_WITH_ALL=ON . build/
```

Documentation is generated in the `doc/html` subdirectory of the build directory. To view it open `index.html` using a web browser, e.g.:
```sh
firefox build/doc/html/index.html
```

## Table of contents

- Getting Started Guides
    - [Getting Started](quickstart.md)
    - [Setting up integrated development environment](setup_ide.md)
    - [How to use Mudita Pure simulator](howto_simulator.md)
    - [Keybindings on Linux](host_keyboard_bindings.md)
    - [Running MuditaOS on a phone](running_on_phone.md)
    - [Windows and macOS - flashing Pure with image](flashing_win_macos.md)
    - [Test harness](../test/README.md)
    - [Tethering](../tethering.md)

- Developer Guides
    - [Coding Guidelines](./MuditaCppCodingGuidelines.md)
    - System architecture
    - Modules
        - Applications
            - [Desktop](../module-apps/application-desktop/doc/README.md)
            - [Notifications](../module-apps/notifications/README.md)
        - [Audio](../module-audio/README.md)
        - [Bluetooth](../module-bluetooth/README.md)
        - Database
            - [Queries](../module-db/queries/README.md)
            - [Tables](../module-db/Tables/README.md)
            - [SMS and Contacts database](database_v2.md)
        - [Filesystem](../module-vfs/README.md)
        - [GUI](../module-gui/README.md)
        - [Modem](../module-cellular/modem/README.md)
        - [System](../module-sys/README.md)
    - Services
        - [Application Manager](../module-services/service-appmgr/doc/README.md)
        - [Bluetooth](../module-services/service-bluetooth/doc/readme.md)
        - [Cellular](../module-services/service-cellular/doc/README.md)
        - [Desktop](../module-services/service-desktop/README.md)
        - [GUI](../module-services/service-gui/doc/README.md)
    - Tools
        - [MapParser](https://github.com/mudita/misc-tools/blob/master/mapparser/README.md)
        - [PureGDB](https://github.com/mudita/misc-tools/blob/master/puregdb/README.md)
- [Development workflow](development_workflow.md)
    - [Creating release](release.md)
- [Contributing](../CONTRIBUTING.md)
- [Internationalization](i18n.md)
- Changelog
    - [HOWTO](changelog_howto.md)
    - [Latest changelog](../changelog.md)
