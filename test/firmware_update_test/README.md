# Firmware update test

The aim of this test is to check the correctness of the `service-desktop` API and upgrade the phone software.

This test is based on [Harness](../README.md) class and uses `pyTest` to verify the update process. It makes use of `service-desktop` 
API to upload a tarball update package to the phone's memory and execute the update. After a successful update the phone should reset
and an update check will be performed. In the end, an update verification status will be shown in the console (along with the exit code).

## Usage
To be able to conduct the test (or just use the script as an update utility) a few prerequisites have to be met:

* Pure needs connected via USB
* Your Python version has to be at least 3.8
* Your Python virtual environment needs to be properly set up (as mentioned in PyTest's [readme](../README.md))

After all of the requirements are fulfilled, the script can be safely executed:

```python
python3 ./update.py tarball_path
```
The serial port used by the phone will be detected automatically.


To obtain the tarball package use the proper CMake target: `make package-update`. The only requirement is having a properly configured API token and Python environment, required by the asset downloading script.
