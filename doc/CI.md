# Continuous Integration

We are using Jenkins as a CI system.
Each internal PR is checked on the CI, and proper status is 
added to the PR.

## Checks that are done
Currently we run several checks:
| Check | Description |
| ---- | ----------- |
|Commit Message check | Check if commit message has proper format (see [Development workflow](development_workflow.md)) |
| Copyright notice check | checks if changed files have proper copyright notice |
| Style checking | Runs clang-format with style described in `.clang-format` against committed code |
| Build RT1051 | Compiles code for RT1051 |
| Build Linux | Compiles code for Linux, runs Unit Tests, and looks for static variables|

## future improvements

- [] Common ccache for all builds
- [] Add Clang-tidy and persistent reports
- [] Daily builds moved to CI
- [] Code Coverage for based on Daily builds

## Jenkinsfile

The current CI pipeline is described in `Jenkinsfile` that is in sources, if a developer needs to 
add some check, it is enough to update the file, the PR will be built with new rules.

In the job description, we may define docker image to use and the parameters that should be set,
but this may require more knowledge, the easiest way is to use `node.label`.
Docker templates are defined directly in Jenkins UI, and this simplifies `Jenkinsfile`

