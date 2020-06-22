# Release

## Create new release

### Checkout commit to make release of
Usually this will be the current master/HEAD

```bash
git checkout master
git pull --ff-only
```

### Run the tests

Run all required tests

### Tag Release
After testing the commit on device you have to tag current commit:

```bash
git tag release-x.y.z
```

Tag has to start with `release-` pefix otherwise it will not be treated
as a commit to create release from.

### Push the Tempo
Then you have to push the tag to the repository:

```bash
git push origin release-x.y.z
```

Pushing the tag to Github will start the release process,
which will try to do following steps:
- check the style
- build linux UT and run them
- build RT1051 and package it
- create draft release on Github
- upload RT1051 package to the release


To track the progress you can look at [actions](https://github.com/muditacom/PurePhone/actions)
After release is successfully build you have to manually publish it on github [releases page](https://github.com/muditacom/PurePhone/releases)

# Delete release:

## Delete the release 
On [release page](https://github.com/muditacom/PurePhone/releases) 
click the release title, in it's subpage on the top right you will 
see the "Delete" button -> click it.

## Delete the tag

### Delete local tag

```bash
git tag --delete release-x.y.z
```

### Delete remote tag

Delete with `--delete` flag

```bash
git push --delete origin release-x.y.z
```

or by refs syntax

```bash
git push origin :refs/tags/release-x.y.z
```

