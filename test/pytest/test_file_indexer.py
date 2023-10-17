# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
from harness.harness import Harness
import pytest
import eyed3
from harness.api.filesystem import put_file

def remove_file(harness, filename):
    body = {"fs": True, "removeFile": filename}
    return harness.endpoint_request("developerMode", "put", body)

def rename_file(harness, filename, destfilename):
    body = {"fs": True, "renameFile": filename, "destfilename": destfilename}
    return harness.endpoint_request("developerMode", "put", body)

def list_files(harness, directory):
    body = {"fs": True, "listDir": directory}
    return harness.endpoint_request("developerMode", "get", body)["body"][directory]

def add_tags(filename, title, artist, album):
    audioFile = eyed3.load(filename)
    audioFile.tag.title = title
    audioFile.tag.artist = artist
    audioFile.tag.album = album
    audioFile.tag.save()
