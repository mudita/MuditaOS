#!/usr/bin/python3
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

'''
Tool for plotting realtime data provided through RTT via JLink.
Data must be wrapped in string containing json with timestamp, current and current_filtered keywords.
Specific line is detected by searching for SCOPE_KEYWORD. All other logs are printed in console.

Usage:
    1. Before running the script JLinkServer must be running
    2. Max number of displayed samples is specified by MAX_SAMPLES_TO_DISPLAY
    3. Plot refresh rate is specified by REFRESH_INTERVAL_MS
    4. To log data into CSV file please provide --save_to_csv FILENAME
'''

import pylink
import sys
import time
import json
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation
import collections 
import argparse
import csv

try:
    import thread
except ImportError:
    import _thread as thread

# Plot constants
MAX_SAMPLES_TO_DISPLAY = 200
REFRESH_INTERVAL_MS = 100

# Parser constants
SCOPE_KEYWORD = '[scope]: '

ts = collections.deque(np.zeros(MAX_SAMPLES_TO_DISPLAY))
meas = collections.deque(np.zeros(MAX_SAMPLES_TO_DISPLAY))
meas_flt = collections.deque(np.zeros(MAX_SAMPLES_TO_DISPLAY))
fig = plt.figure(figsize=(12,6))
ax = plt.subplot()
csv_filename = str()

def save_to_csv(to_save):
    # save to csv if filename provided
    if csv_filename:
        with open(csv_filename, 'a') as file:
            writer = csv.writer(file)
            writer.writerow(to_save)

def parse_lines(charbuf):
    for line in charbuf.splitlines():
        if SCOPE_KEYWORD in line:
            try:
                to_parse = line.split(SCOPE_KEYWORD)[1]
                json_meas = json.loads(to_parse)

                ts.popleft()
                ts.append(int(json_meas['timestamp']))
                meas.popleft()
                meas.append(int(json_meas['current']))
                meas_flt.popleft()
                meas_flt.append(int(json_meas['current_filtered']))

                save_to_csv([int(json_meas['timestamp']), int(json_meas['current']), int(json_meas['current_filtered'])])
            except:
                print("exception in json parse, corrupted data")
        else:
            print(line) # print other logs in console

def read_rtt(jlink):
    try:
        while jlink.connected():
            terminal_bytes = jlink.rtt_read(0, 1024)
            if terminal_bytes:
                parse_lines("".join(map(chr, terminal_bytes)))
            time.sleep(0.01)
    except Exception:
        print("IO read thread exception, exiting...")
        thread.interrupt_main()
        raise

def animation(i):
    try:
        ax.cla()
        num_of_elems = int(np.nonzero(ts)[0][0]) # find first nonzero element in ts queue
        ax.plot(ts, meas, 'b')
        ax.plot(ts, meas_flt, 'g')

        # plot styling
        plt.title("Current measurement - b: inst, g: filtered", loc = 'left')
        plt.xlabel("Timestamp [ms]")
        plt.ylabel("Current [mA]")
        plt.xlim([ts[num_of_elems], ts[-1]])
        plt.grid(True)
    except:
        print("exception in plotter - no data provided")

def main():
    jlink = pylink.JLink()
    print("connecting to JLink...")
    jlink.open()
    print("starting RTT...")
    jlink.rtt_start()

    ani = FuncAnimation(fig, animation, interval=REFRESH_INTERVAL_MS)
    
    try:
        thread.start_new_thread(read_rtt, (jlink,))
        plt.show()
        while jlink.connected():
            time.sleep(1)
        print("JLink disconnected, exiting...")
    except KeyboardInterrupt:
        print("ctrl-c detected, exiting...")
        pass

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="RTT realtime plotter")
    parser.add_argument("--save_to_csv",
        help="Save data to csv file. Provide filename as an argument.",
        default='')

    args = parser.parse_args()
    csv_filename = args.save_to_csv

    sys.exit(main())
