# Assumptions

* It's not possible to type numbers with `+` prefix like `+5`
* It's not possible to type operation like `5--5`. Instead, use `5+5`
* Dividing by 0 will result in displaying `Error` message
* Input is limited to the 7 characters including
* Maximal input value can be set to `9999999` or `99999.9`
* Minimal input value can be set to `-999999` or `-9999.9`
* Press `*` to remove the last typed input. It can be used multiple times to revert more than one input value.
* It is possible to recall the last typed operation by using the `*` key. First you have to clear the current input, and
  then press the `*` once again.

```
123+
322 <- start pressing `*`
32
3
    <- empty input field, press the `*` once again
123+
```

* Trying to execute operations without right operand will trigger displaying `Error` message, i.e. `100+=`
* There is no support for typing values using scientific notation, i.e. `10e9`
