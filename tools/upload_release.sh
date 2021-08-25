#!/bin/bash
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# base taken from: https://gist.github.com/stefanbuck/ce788fee19ab6eb0b4447a85fc99f447 and modified
# modman: Adam
# license: mit

set -eou pipefail

for line in "$@"; do
  eval "$line"
done

[ -v "tag"  ]  ||   { echo "script requires tag as parameter i.e. latest release tag"; exit 1; }
[ -v "token" ] ||  { echo "script requires token as parameter to access server (please see GitHub tags)"; exit 1; }
[ -v "files" ] ||  { echo "script requires files as parameters to know what files to upload to server"; exit 1; }
[ -v "repo" ]  ||  { echo "script requires repo to push to"; exit 1; }

github_api_token="$token"
filenames=( $(echo $files | tr "," " ") )

for file in "${filenames[@]}"; do
    [ -f "$file" ] || { echo "file '$file' doesn't exist!"; exit 1; }
done

# Define variables.
GH_REPO="https://api.github.com/repos/mudita/$repo"
GH_TAGS="$GH_REPO/releases/tags/$tag"
GH_RELEASE="$GH_REPO/releases"
AUTH="Authorization: token $github_api_token"

echo "test token"
curl -o /dev/null -sH "$AUTH" $GH_REPO || { echo "Error: Invalid repo, token or network issue!";  exit 1; }

echo "create release"
curl -sH "$AUTH" -H "Accept: application/vnd.github.v3+json" -X POST $GH_RELEASE -d '{"tag_name":"'"$tag"'", "prerelease":true, "name":"'"$tag"'","body":"WARNING: this is daily prerelease"}' &>/dev/null

response=$(curl -sH "$AUTH" $GH_TAGS)
eval $(echo "$response" | grep -m 1 "id.:" | grep -w id | tr : = | tr -cd '[[:alnum:]]=')
[ "$id" ] || { echo "Error: Failed to get release id for tag: $tag"; echo "$response" | awk 'length($0)<100' >&2; exit 1; }

echo "release ID : ${id}"


for filename in "${filenames[@]}"; do
    GH_ASSET="https://uploads.github.com/repos/mudita/$repo/releases/$id/assets?name=$(basename $filename)"
    [[ "$filename" =~ .*.tar ]] && content_type='Content-Type: application/x-tar' || content_type='Content-Type: application/x-xz'
    curl -s --data-binary @"$filename" -H "Authorization: token $github_api_token" -H "$content_type" $GH_ASSET  &>/dev/null
    echo "asset $filename added to release under: tag $tag"
done

echo "uploading files done!"
