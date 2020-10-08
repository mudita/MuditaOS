# How to create and delete a MuditaOS release

## Creating a new release

1. Checkout the commit to make the release of - usually this will be the current `master/HEAD`

```bash
git checkout master
git pull --ff-only
```

2. Run all required tests

3. Tag the release. After testing the commit on a device you have to tag current commit:

```bash
git tag release-x.y.z
```

Release tag has to start with `release-` prefix, otherwise it will not be treated as a commit to create the release from.

4. Push the tag to the repository:

```bash
git push origin release-x.y.z
```

Pushing the tag to GitHub will start the release process, which will try to complete the following steps:

- check the style
- build linux UT and run them
- build RT1051 and package it
- create draft release on GitHub
- upload RT1051 package to the release

To track the progress you can look at [GitHub actions](https://github.com/mudita/MuditaOS/actions)

After the release is successfully build you have to manually publish it on GitHub [releases page](https://github.com/mudita/MuditaOS/releases)

## Deleting an existing release

1. Delete the release by clicking on the release title on the [release page](https://github.com/mudita/MuditaOS/releases). In its subpage in the top right corner you will see the `Delete` button -> click it.

2. Delete the tags.

 - Delete local tag: 

```bash
git tag --delete release-x.y.z
```

 - Delete remote tag: 

```bash
git push --delete origin release-x.y.z
``` 
OR

```bash
git push origin :refs/tags/release-x.y.z
```