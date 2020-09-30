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

- [Setting up development environment](#)
    - [Quickstarts](#)
        - [Quickstart using Docker](#)
        - [Quickstart in your local environment](#)
        - [Super dirty quickstart on Linux](#)
        - [Super dirty quickstart with unit tests](#)
        - [Super dirty quickstart on the phone](#)
        - [Longstart](#)
        - [Building your own Docker image](#)
    - [Setting up integrated development environment](setupide.md)
    - [Running MuditaOS on a phone](#)
    - [GUI](#)
    - [Keybindings on Linux](#)
    - [Tests](#)
- [Development workflow](#)
    - [Creating release](#)
- [Contributing](#)
- [Modules](#)
    - [Bluetooth](#)
    - [lwip](#)
- [SMS and Contacts database](#)
- [Changelog](#)
    - [HOWTO](#)
    - [Latest changelog](#)