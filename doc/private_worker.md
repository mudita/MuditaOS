# Private Github Worker


## Create Image

First create docker image by calling
```bash
./config/build_runner_docker
```
This will create container "pure_builder:latest".

## Get a GitHub token

[Add a token for runner in actions section](https://github.com/muditacom/PurePhone/settings/actions) and press "Add Runner"

## Configure Container

```bash
pushd config
cp runner_settings.in runner_settings
vim runner_settings  #<- update WORKER_TOKEN variable
popd
```

## Run The Worker

```bash
./config/run_runner_docker
```

