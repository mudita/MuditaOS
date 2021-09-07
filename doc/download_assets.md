# Download Assets

Building update packages requires some external assets.
For now we need only `ecoboot.bin` and `PureUpdater_RT.bin`.
The files may be downloaded manually from [ecoboot](https://github.com/mudita/ecoboot/releases) and [PureUpdater](https://github.com/mudita/PureUpdater/releases) release pages, respectively, from the "Assets" section.
This requires the user to log in to GitHub and click inside the web browser, which is a "no go" for automated builds.
To automate this process we have introduced a tool `tools/download_asset.py`.

## GitHub API token

To use GitHub API your GitHub login and special token (API password) are required.
This token can by generated on "User" -> [Settings](https://github.com/settings/profile) page in section [Developer settings](https://github.com/settings/apps) -> [Personal acces tokens](https://github.com/settings/tokens) section.
For more info please refer to the Github help page about adding tokens: [Creating a personal access token](https://docs.github.com/en/free-pro-team@latest/github/authenticating-to-github/creating-a-personal-access-token)
### Scopes
When you generate a token you can set "Scopes" for which the token will work.
For downloading assets, we need a ":repo" scope with all sub-scopes.

**Note:** Tokens are displayed only during their creation, so this is the only moment you can copy them.
If you forget to do this, you will have to generate a new token.

## Storing settings
The `download_assets.py` file can have `login` and `password` passed through parameters,
or this data can be stored in your current git repository configuration.

Adding GitHub login to:
```bash
git config --add user.githublogin <login>
```

Adding API Token:
```bash
git config --add user.apitoken
```

Checking the values:
```bash
git config user.githublogin
git config user.apitoken
```

## How to use `download_asset.py`

```bash
$./tools/download_asset.py --help
usage: download_asset.py [-h] [-w WORKDIR] [-t TOKEN] [-l LOGIN] repository {list,ll,download,dw} ...
```

To work properly, `download_asset.py` requires <Login> and <Token>. If they are stored in repo config these
parameters are not required, otherwise, our tool will protest if parameters are not set.
If parameters are set in `.git/config` and You still add these values, they will override the ones in the config.

This tool downloads assets only from our "mudita" organisation in GitHub, but we can change the repository,
this is `repository` parameter, the second required parameter is a task to be completed:
Tasks:
* `list` or `ll`     - list releases, first column is a tag, which may be used in download command.
* `download` or `dw` - downloads first asset in release (if no tag is added, the latest release is used).

## Usage example

List releases:
```bash
./tools/download_asset.py ecoboot ll
 tag | name | date | pre-release
--------------------------------
1.0.4 | Bug fixes for checksum handling | 2020-08-27T13:26:11Z | True
1.0.1 |  | 2020-08-12T09:45:53Z | False
1.0.3 | 1.0.3 JSON to ini migration | 2020-07-28T13:29:56Z | False
0.1 | Support for 2 partitions and boot.ini | 2020-07-07T14:48:18Z | False
05_07_2019 | Working with PurePhone | 2019-07-05T11:36:51Z | True
```

Download latest `ecoboot.bin`
```bash
./tools/download_asset.py ecoboot dw
```

Download `ecoboot.bin` for tag `1.0.3`
```bash
./tools/download_asset.py ecoboot dw 1.0.3
```

Download the latest MuditaOS package to the `packages` directory with a custom login and API token
```bash
./tools/download_asset.py -w packages -l someUser -t secreetToken123123123 MuditaOs dw
```
