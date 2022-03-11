How to generate documentation using Doxygen
=============================================

**NOTE:** doxygen documentation is just another target.
Basic targets documentation can be found [here](./build_targets.md).
How to setup environment to be able to build the targets is here: [quickstart](./quickstart.md)


Fully detailed doxygen documentation can be built locally using [Doxygen](https://www.doxygen.nl/index.html).

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


