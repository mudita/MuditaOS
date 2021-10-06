MuditaOS misc tooling
=====================

# Installing packages

```
python3 -m pip install -r requirements.txt
```

# Updating packages

1. Create venv with curent setup:  
```
python3 -m venv venv
source venv/bin/activate
python3 -m pip install -r requirements.txt
```

2. Install required packages  

3. Freeze requirements:  
```
pip freeze > requirements.txt
```

# Testing for proper asset download configuration:

0. check if you have token configured

```
git config user.apitoken
```

**if not**  please add token ( see doc: [documentation](../doc/download_asset.md)

1. check if you have access to PureUpdater repository in read mode

```
curl -o /dev/null -H "Authorization: token $(git config user.apitoken)" "https://api.github.com/repos/mudita/PureUpdater"
```
**if not** Then please either fix your token access, or request access to PureUpdater

2. Check if you can list releases on repository:
```
python3 ./tools/download_asset.py -w ./ --repository PureUpdater list                                                                                                                                             21s[cbe063dc6]
```

**if not** Then fix your token, or request access to PureUpdater
