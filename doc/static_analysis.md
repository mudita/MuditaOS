# Static code analysis

The Static code analysis is a method of debugging that is done by examining the source code without executing the program.

Before submitting the Pull Request a developer should run static code analyzer tool and make sure that his code complies with the Coding Standards.

## Clang-tidy

The clang-tidy is a clang-based C++ “linter” tool. Its purpose is to provide an extensible framework for diagnosing and fixing typical programming errors, like style violations, interface misuse, or bugs that can be deduced via static analysis.

### Install clang-tidy

#### Linux Ubuntu

The clang-tidy tool is available from the APT repository.

`$ sudo apt install clang-tidy`

### Run clang-tidy

If `run-clang-tidy` program is installed, a CMake target called `clang-tidy` is available.

In order to run `clang-tidy` analyzer on the MuditaOS repository, follow the steps:
```
$ cd <path/to/MuditaOS>
$ ./configure.sh <platform> <build_type>
$ cd <path/to/build/dir>
$ make clang-tidy
```

The results of the analysis are available under `$(pwd)/StaticAnalysis` directory.
