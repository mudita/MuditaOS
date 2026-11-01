# Regression test plan: ENABLE_USB_TASK=ON

Validates that routing USB device notifications through a dedicated
FreeRTOS task (`USB_DEVICE_CONFIG_USE_TASK=1`, enabled via the
`ENABLE_USB_TASK` CMake option) is functionally equivalent to the
default ISR-routed path, and that none of the seven issues fixed in
the `[MOS-000] Make USB_DEVICE_CONFIG_USE_TASK=1 path safe to enable`
commit regresses.

Each scenario must be executed against two builds:

  * **Build A** - `ENABLE_USB_TASK=OFF` (baseline, current production path)
  * **Build B** - `ENABLE_USB_TASK=ON`  (new path under validation)

Pass criteria is that both builds produce the same host-observable
result; trace differences are expected only for the `usb_device` task
itself (absent in A).

## Conventions

* **HUT** (Hardware Under Test): Mudita Pure with current Harmony OS,
  battery >=50%, T 20-25 C, original USB-C cable.
* **Host**: Linux Ubuntu 22.04 with `usbutils`, `mtp-tools`,
  `screen`/`picocom`, `dd`, `sha256sum`.
* Logging via J-Link RTT (`JLinkRTTLogger`); host-side via
  `dmesg -w`, `journalctl -u systemd-udevd`.

## Required hardware / tooling

* 2x Mudita Pure with current firmware
* J-Link + RTT viewer (or `JLinkRTTLogger`)
* Programmable USB hub with `uhubctl` (scenarios 6, 7)
* DCP / CDP / SDP power sources (scenario 8)
* Automation scripts (bash/python) using `mtp-tools`, `picocom`, `pyusb`

---

## Scenario 1 - Bring-up after cold boot

**Goal:** validate init / deinit (fix step 1 - deinit order and typo)
on both ON and OFF paths.

**Setup:** HUT disconnected from USB, powered off.

**Steps:**
1. Power on HUT, wait for clock screen.
2. Plug USB into host.
3. On host: `lsusb | grep 3310` - expect VID 3310, PID 0100
   (PurePhone with MTP).
4. `dmesg -w` should show `usb X-Y: new high-speed USB device`, no
   `device descriptor read/64, error -...`, no
   `device not accepting address`.
5. `journalctl -u systemd-udevd` - no errors.
6. Unplug USB.
7. Repeat steps 2-6 ten times with varying plug/unplug delays
   (0.5 s, 2 s, 5 s, 10 s).

**Pass criteria:**
* Each plug -> device enumerated within 2 s.
* No `Cannot connect`, no HUT crash (LED, watchdog reset).
* In B: log `[Composite] USB initialized` accompanied by USB device
  task creation trace.

**Regression probes:** fix step 1 (typo `composite_deinit`), fix
step 1 (order SetIsr -> Stop -> TaskDelete).

---

## Scenario 2 - CDC echo loop (CDC driver sanity)

**Goal:** validate `VirtualComSend` / `VirtualComRecv` /
`OnSendCompleted` / `OnRecvCompleted` paths (fix steps 3-4 - txMutex
vs taskENTER_CRITICAL).

**Setup:** HUT with Developer Mode enabled - CDC ACM appears as
`/dev/ttyACM0`.

**Steps:**
1. On host: `picocom -b 115200 /dev/ttyACM0`.
2. Send an echo / minimal endpoint enumeration command, e.g.
   `{"endpoint":1,"method":1,"uuid":1}\r`.
3. Repeat 1000 times in a scripted loop with 200 ms read timeout
   and response CRC check.
4. Concurrently: thread 2 sends frames every 50 ms; thread 3 reads
   non-blocking.

**Pass criteria:**
* 1000/1000 responses received, CRC matches.
* No `[VCOM] Error: dropped` in RTT log.
* No deadlock (test finishes within 90 s).

**Regression probes:** `VirtualComSend` race vs `OnSendCompleted`
(fix step 4); no byte loss in `mudita_vcom_stream_send` (fix step 3).

---

## Scenario 3 - FilesystemUpload >=1 MiB (heavy CDC traffic)

**Goal:** stress one-directional CDC, validate outputStream buffer
(512 B chunks).

**Setup:** Developer Mode `filesystemUpload` endpoint.

**Steps:**
1. Generate 1 MiB random file: `dd if=/dev/urandom of=test1m.bin bs=1M count=1`.
2. `sha256sum test1m.bin` -> `H1`.
3. Upload via `filesystemUpload` to `/user/test1m.bin`.
4. Download the same file via `filesystemDownload` -> `out.bin`.
5. `sha256sum out.bin` -> `H2`.
6. Repeat with 4 MiB.

**Pass criteria:**
* `H1 == H2`.
* Transfer time in B <= 1.2x transfer time in A (USB task should not
  meaningfully slow down throughput - all other FreeRTOS priorities
  unchanged).
* No `USB_EVENT_ERROR_RX_BUFFER_OVERFLOW` /
  `USB_EVENT_ERROR_TX_BUFFER_OVERFLOW` in RTT.

**Regression probes:** fix step 3 (`mudita_vcom_stream_send` returns
correct byte count); fix step 4 (mutex does not throttle throughput).

---

## Scenario 4 - MTP browse / idle (MTP sanity)

**Goal:** validate `MtpTask`, MTP message queue, OnIncomingFrame /
OnOutgoingFrameSent callbacks (fix step 5).

**Setup:** GVFS / mtp-tools on host.

**Steps:**
1. `mtp-detect` on host.
2. `gio mount mtp://...` (or Nautilus).
3. Browse 4 directories with 3 levels of nesting.
4. `mtp-files` - list all objects.
5. Idle 5 min with mount open.
6. Unmount.

**Pass criteria:**
* Each directory renders within 1 s.
* No `[MTP] FATAL`, `[MTP] Dropped outgoing bytes`, `[MTP] RX dropped`.
* Idle: no CPU wakeups (SystemView if available).
* Clean unmount; in B `MtpDeinit` log in the correct order (fix step 1).

**Regression probes:** fix step 5 (`mudita_mtp_msg_send/recv`
wrappers); fix step 5 (`MTP_TASK_PRIORITY=Idle+2` - task visible in
trace).

---

## Scenario 5 - MTP copy >=10 MiB (MTP stress)

**Goal:** prove that raising MTP task priority (`Idle+2` in the fix)
does not starve the system and that bi-directional messageBuffer
wrappers are transparent.

**Setup:** 10 MiB file.

**Steps:**
1. Copy host -> HUT: `cp test10m.bin /run/user/$UID/gvfs/mtp:.../INTERNAL/`.
2. `sha256sum` on host and after copying back to host.
3. Repeat 3 times.
4. Concurrently: HUT screen activity (menu scrolling) - should not
   freeze.

**Pass criteria:**
* 3/3 transfers with identical SHA-256.
* HUT UI responsive (subjectively <= 200 ms latency per press).
* No `[MTP] Incomplete transfer`.

**Regression probes:** fix step 5 (MTP task priority); fix step 4
(WorkerDesktop responsiveness - it runs at Idle and must not be
permanently preempted).

---

## Scenario 6 - Replug under load

**Goal:** validate `composite_deinit` when the USB task has in-flight
data.

**Setup:** scenario 5 running, 10 MiB copy in progress.

**Steps:**
1. Mid-transfer (~5 MiB) unplug the USB cable.
2. Check RTT log - no `nullptr deref`, no `vStreamBufferDelete on
   busy buffer`, no watchdog reset.
3. Wait 3 s, plug back in.
4. Check enumeration (as in scenario 1).
5. Repeat 5 times.

**Pass criteria:**
* HUT does not reset (LED stays on).
* After re-enumeration MTP/CDC work.
* Host: `dmesg` may show `bulk endpoint stall` but not
  `device descriptor read failed`.

**Regression probes:** fix step 1 (deinit order: SetIsr(false)
before vStreamBufferDelete); fix step 2 (`device_task_handle` deleted
correctly).

---

## Scenario 7 - Suspend / resume

**Goal:** USB suspend events do not crash the task.

**Setup:** HUT plugged into a USB hub.

**Steps:**
1. `echo suspend > /sys/bus/usb/devices/X-Y/power/level`
   (or `uhubctl -a off -p N`).
2. Wait 30 s.
3. Resume: `... -a on ...`.
4. Verify that MTP/CDC work.
5. Repeat 3 times.

**Pass criteria:**
* No crash, no state loss.
* `MtpReset` log after resume (both A and B).

**Regression probes:** fix step 1 (deinit/init flow). Does not
directly exercise fix step 6.

---

## Scenario 8 - Battery DCD detect

**Goal:** USB charger detect (BC1.2) is independent of the USB task -
sanity that we did not break it.

**Setup:** DCP charger, CDP hub, SDP port (PC).

**Steps:**
1. Power off HUT.
2. Connect each of the 3 source types.
3. Verify `[DCD]` log or UI (charging icon shows DCP/CDP/SDP).
4. Power on HUT while connected to SDP, verify enumeration.

**Pass criteria:** Identical detection in A and B.

**Regression probes:** fix step 1 (DCD path runs outside our change
but shares NVIC priority).

---

## Scenario 9 - 24 h stress

**Goal:** long-term stability.

**Setup:** host-side automation looping scenarios 2 and 5 alternately.

**Steps:**
1. Script: every 60 s randomly pick an action (CDC echo 100 frames /
   MTP copy 1 MiB / idle 30 s / replug).
2. Log all RTT errors to a file.
3. Run for 24 h.

**Pass criteria:**
* Zero crashes.
* Zero `[VCOM] Error`, `[MTP] FATAL`.
* Heap watermark stable (check `xPortGetMinimumEverFreeHeapSize`
  before / after - difference <= 5 %).

**Regression probes:** all 7 fixes combined, leak hunting.

---

## Scenario 10 - Equality OFF vs ON

**Goal:** prove that mode B is functionally equivalent to A from the
host's perspective.

**Setup:** Two identical HUTs, A=OFF, B=ON.

**Steps:**
1. Run scenarios 1, 2, 3, 4, 5 sequentially on A.
2. Repeat on B with the same input data.
3. Compare:
   * transfer times (Welch t-test, p > 0.05)
   * counts of error/dropped bytes
   * file SHA-256
   * enumeration time (`dmesg` timestamps)

**Pass criteria:**
* SHA-256 identical.
* Times within 20 % of each other.
* No `error` logs on either side.

**Regression probes:** cumulative proof of functional equivalence
(PR acceptance criterion).

---

## Fix coverage matrix

| Fix (implementation step)                  | Covered by scenarios |
| ------------------------------------------ | -------------------- |
| 1. `composite_deinit` typo + order         | 1, 6, 7              |
| 2. USB device task priority / stack / name | 5, 9                 |
| 3. `vcom_buffers.h` wrappers               | 2, 3                 |
| 4. `txMutex` in CDC                        | 2, 3, 9              |
| 5. MTP wrappers + priority                 | 4, 5, 9              |
| 6. `IsBusy` documentation                  | (review, not runtime)|
| 7. `ENABLE_USB_TASK` config comment        | (review, not runtime)|
| Profile.hpp fix (separate PR)              | all (precondition for full rt1051 build) |

## PR acceptance criteria

* Scenarios **1, 2, 3, 4, 5, 10** must pass for both builds.
* Scenarios **6, 7, 8** must pass for build B (A is verified to
  confirm no regression).
* Scenario **9** (24 h) is optional for the MR, mandatory before
  flipping `ENABLE_USB_TASK=ON` as the default in any release.
