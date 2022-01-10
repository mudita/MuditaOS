# Continuous Integration

We are using Jenkins as a CI system.
Each internal PR is checked on the CI, and proper status is added to the PR.

## Checks that are done
Currently we run several checks for each target:
| Check | Description |
| ---- | ----------- |
| Commit Message check | Check if commit message has proper format (see [Development workflow](development_workflow.md)) |
| Copyright notice check | checks if changed files have proper copyright notice |
| Style checking | Runs clang-format with style described in `.clang-format` against committed code |
| Build RT1051 | Compiles code for RT1051 for bell and pure targets |
| Build Linux | Compiles code for Linux, runs Unit Tests, and looks for static variables for bell and pure targets|

## what we have

* all steps are written down in `Jenkinsfile`, these steps use our build library on Jenkins
* we already have:
    * daily builds on CI from which daily releases artifacts are created
    * release builds on CI from which release artifacts are created
    * ccache'd builds
* drafts & multiple requests for builds are dropped ( to not choke the build's queue)
* builds are in docker in VM
* build status is reported on GitHub

# issues and requests for improvements

Please raise issue in github with full description what's wrong or what would you like to be improved.
