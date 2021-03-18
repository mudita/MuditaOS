# MuditaOS Documentation

Thid folder contains everything you need to kickstart your development environment for MuditaOS.

A fully detailed documentation can be build locally using [Doxygen](https://www.doxygen.nl/index.html).

## How to generate documentation using Doxygen

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

- [Quickstarts](quickstart.md)
    - [Quickstart using Docker](quickstart.md#quickstart-using-docker)
    - [Building your own Docker image](quickstart.md#building-your-own-docker-image)
    - [Quickstart in your local environment](quickstart.md#quickstart-in-your-local-environment)
    - [Longstart](quickstart.md#longstart)
    - [Super dirty quickstart on Linux](quickstart.md#super-dirty-quickstart-on-linux)
    - [Super dirty quickstart with unit tests](quickstart.md#super-dirty-quickstart-with-unit-tests)
    - [Super dirty quickstart on the phone](quickstart.md#super-dirty-quickstart-on-the-phone)
    - [Preparing packages](quickstart.md#preparing-packages)
- [Setting up integrated development environment](setup_ide.md)
- [Test harness](../test/README.md)
- [Running MuditaOS on a phone](running_on_phone.md)
- [Windows and macOS - flashing Pure with image](flashing_win_macos.md)
- [GUI](../module-gui/README.md)
- How to use Mudita Pure simulator(howto_simulator.md)
- [Keybindings on Linux](host_keyboard_bindings.md)
- [Development workflow](development_workflow.md)
    - [Creating release](release.md)
- [Contributing](../CONTRIBUTING.md)
- [Internationalization](i18n.md)
- Modules
    - [Bluetooth](../module-bluetooth/README.md)
    - [lwip](../module-lwip/README.md)
- [SMS and Contacts database](database_v2.md)
- Changelog
    - [HOWTO](changelog_howto.md)
    - [Latest changelog](../changelog.md)