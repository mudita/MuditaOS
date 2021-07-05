# Adding Unit Test

## Unit test framework

For writing test we are using (catch2)[https://github.com/catchorg/Catch2/blob/devel/docs/tutorial.md].
There are few test written in `gtest` but we focus on using `catch2`

## adding unit test

In `CMakeLists.txt` use `add_catch2_executable` function for tests based on `catch2`.
If you wrote your test in `gtest` use `add_gtest_executable` function.
Both functions are defined in `test/CMakeLists.txt` and there described parameters.
Then to make sure that test is run add it to `enabled_unittests`.

Each binary has it's own section, and in section we each test in separate line,
each test name have to end with;

## disable unit test

If for some reason you need to disable some unit test, find it in `enabled_unittests` and
put # before it's name

## checking what tests are run

On our CI, we check what tests are available in binaries and if they where run.
If some tests are not run, job called "Check run unit tests" will fail, this check
is not required for PR to pass, it is just indicator that some tests are not working.

## running tests

Locally we may run tests in two way by `make check` or by `./tools/run_unittests.sh`

### make check

This will run all defined tests, even disabled ones, what is useful for unit test debugging.
This way is not used on CI.

```bash
cd buil-linux-Debug
make check
```

### run_unittests.sh

Runs only enabled tests, this how we do it on CI.
All tests must pass, if test doesn't pass PR is blocked.


```bash
./tools/run_unittests.sh enabled_unittests
```

This script may be used for running set of tests.
