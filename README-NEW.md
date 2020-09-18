# MuditaOS

MuditaOS is a mobile operating system optimized for E Ink displays. Built from scratch with Mudita Pure minimalist phone in mind.

![MuditaOS screenshot]()

## Table of contents

* [Quickstart](#)
* [Contributing]
   * [Discussions](#)
   * [Reporting issues and feature requests](#)
   * [Development workflow](#)
* [Documentation](#documentation)
   * [How to generate documentation using Doxygen](#)    
* [License](#license)

## Quickstart

You can quickstart the project by going through one of the following guides:

- [Quickstart using Docker]()
- [Quickstart in your local environment]()
- [Super dirty quickstart on Linux]()
- [Super dirty quickstart with unit tests]()
- [Super dirty quickstart on the phone]()
- [Longstart]()
- [Building your own Docker image]()

## Contributing

Pull requests are welcome. Please follow the guidelines in the ["Contributing to MuditaOS"](#) article.

### Discussions

[Stack Overflow](#) is the best place for developers to share their problems and solutions with the community. Please make sure to tag your questions with `muditaos` to reach the widest audience. We’ll do our best to address your issues by answering them promptly on Stack Overflow.

For general questions and ideas regarding MuditaOS please post in the [“MuditaOS Suggestions” thread on Mudita Forum](#). Please explore all existing posts to make sure you’re not posting about an issue that has already been discussed.

### Reporting bugs and feature requests

You can report bugs and feature requests on [GitHub](#). This is also a good place to discuss architecture decisions and things that aren’t yet covered by the documentation. Please refer to the ["Contributing to MuditaOS"](#) article for more details.

### Development workflow

When contributing code or documentation changes please follow the guidleines inside the ["Development workflow"](#) article.

## Documentation

For everything that you need to kickstart your development environment please go to the [Documentation](#) folder on Github.

A fully detailed documentation can be build locally using [Doxygen](https://www.doxygen.nl/index.html).

### How to generate documentation using Doxygen

You can build code documentation with the following command:
```sh
./configure linux Debug
cd build-linux-Debug
make doc
```
If you prefer to build documentation always with `all` target instead, configure `cmake`
with `BUILD_DOC_WITH_ALL` option set to `ON`, e.g.:
```sh
./configure <target> <build_type> -DBUILD_DOC_WITH_ALL=ON
```

Documentation is generated in the `doc/html` subdirectory of the build directory. To view it open `index.html` using a web browser, e.g.:
```sh
firefox build/doc/html/index.html
```

## Changelog

The [MuditaOS changelog](#) is regularly updated by the core development team.

## License
MuditaOS is licensed on [GPL3](https://choosealicense.com/licenses/gpl-3.0/).
