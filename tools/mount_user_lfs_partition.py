# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import argparse
import os

parser = argparse.ArgumentParser(
    usage=
"\ne.g.:\n"
"For         simulator: python3 tools/mount_user_lfs_partition.py --mount_dir /mnt/user --lfsfuse build-linux-Debug/lfsfuse --image build-linux-Debug/PurePhone.img --loop 1\n"
"For device (MSC mode): python3 tools/mount_user_lfs_partition.py --mount_dir /mnt/user --lfsfuse build-rt1051-RelWithDebInfo/lfsfuse --part3_path /dev/sda3")

parser.add_argument('--mount_dir', type=str,
                    help="Directory where LFS should be mounted", required=True)
parser.add_argument('--lfsfuse', type=str,
                    help="Path to `lfsfuse`", required=True)
parser.add_argument('--part3_path', type=str, help="Path to third partition (when mounting PURE LFS) e.g. `/dev/sda3`")
parser.add_argument('--image', type=str, help="Path to PurePhone.img (when mounting LINUX IMAGE LFS)")
parser.add_argument('--loop', type=int,
                    help="Loop device number - different than already existed, see `losetup --list` (when mounting LINUX IMAGE LFS)")
args = parser.parse_args()

if args.loop and args.image:
    command = "sudo losetup -P /dev/loop" + str(args.loop) + " " + str(args.image)
    print("Executing command:", command)
    result = os.system(command)

    if result != 0:
        os._exit(result)

    command = "sudo chmod 666 /dev/loop" + str(args.loop)
    print("Executing command:", command)
    result = os.system(command)

    if result != 0:
        os._exit(result)

elif not args.part3_path:
    print("ERROR! part3_path arg not provided!")
    os._exit(-1)


if not os.path.exists(args.mount_dir):
    print("Mount dir not exist! Trying to create it...")
    command = "sudo mkdir " + args.mount_dir
    print("Executing command:", command)
    result = os.system(command)

    if result != 0:
        os._exit(result)

    if not os.path.exists(args.mount_dir):
        print("Failed to create mount dir!")
        os._exit(-1)
    else:
        print("Mount dir created!")

command = "sudo " + args.lfsfuse + " "
if args.loop:
    command += "/dev/loop" + str(args.loop) + "p3 "
else:
    command += args.part3_path + " "

command += args.mount_dir + " --block_size=32768"
print("Executing command:", command)
result = os.system(command)

if result != 0:
    os._exit(result)

print("Procedure completed. Please check " + args.mount_dir)
