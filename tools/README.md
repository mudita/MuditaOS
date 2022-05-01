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

# Downloading assets

To download assets required to build release we have `download_asset.py` tool which is documented here:
[/doc/download_assets.md](/doc/download_assets.md)
