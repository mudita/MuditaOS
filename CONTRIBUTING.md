# Contributing to MuditaOS

MuditaOS is an Open Source project and we're more than happy to receive contributions from our community. There are many ways in which you can contribute to the project from reporting bugs, suggesting features to writing code and documentation.

Before contributing please read and accept our [Code of Conduct](./CODE_OF_CONDUCT.md).

**Note:** You will be able to test your contributions only on [Mudita Pure Simulator](./doc/howto_simulator.md). You won't be able to produce a build for Mudita Pure yourself, since the operating system contains a a piece of proporietary software from E Ink Holdings Inc., namely `luts.bin` - a small program the E Ink display executes each time you refresh it.

## Reporting bugs

If you think you found a bug or a security issue in MuditaOS, we'd love to hear from you. Before you create a relevant GitHub issue please:

- make sure you're using the latest MuditaOS release when spotting the bug by heading to the `Settings` -> `Information` section on your Phone or emulator. Here you can see the all the details about the version of MuditaOS you're currently using. Please compare it with the latest stable release of MuditaOS which is always available at the top of our [changelog](./changelog.md) and in [Releases section on GitHub](https://github.com/mudita/MuditaOS/releases).

- make sure that the issue hasn't been already reported on GitHub issues (if it has, please add a 👍 reaction to it).

### Reporting a bug - template

Please make sure that your bug report includes the following data:

- General description (what happened)
- Expected behavior (what you think should happen)
- Current behavior (current 'buggy' behavior)
- Steps to reproduce the bug

Feel free to add any screenshots or gifs/screencasts to illustrate the issue.

## Feature requests

If you have an idea for a feature that could possibly land in MuditaOS please let us know by simply creating an issue on GitHub in which you **describe the feature**, **why it would be useful** and **how it should work**. Before you create a Feature request on GitHub please:

- make sure that the feature request hasn't been already reported on GitHub issues (if it has, please add a 👍 reaction to it).

## Contributing code and documentation changes

If you would like to contribute a new feature or a bug fix to MuditaOS, please discuss your idea first in GitHub issues. If there is no Github issue for your idea, please open one. It may be that somebody from Mudita core team is already working on it, or that there are particular complexities that you should know about before starting the implementation.

Please refer to the ["Development workflow" article](./doc/development_workflow.md) to learn how to submit code and documentation changes to the MuditaOS repository.

If you want to start localizing MuditaOS interface please start from [the "Internationalization" article](./doc/i18n.md).
