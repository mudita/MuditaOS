# Async USB task fix - change summary

Summary of the changes that make the asynchronous USB device path
(`USB_DEVICE_CONFIG_USE_TASK=1`, exposed through the `ENABLE_USB_TASK`
CMake option) safe to enable. The work spans the main repository and
the `third-party/usb_stack` submodule, and is split across three
independent branches / pull requests.

## Background

By default MuditaOS drains USB device notifications directly inside
the EHCI interrupt handler (`USB_OTG1_IRQHandler`, NVIC priority 3).
The USB stack also ships an alternative routing
(`USB_DEVICE_CONFIG_USE_TASK=1`) in which the ISR only enqueues an
event and a dedicated FreeRTOS task performs the actual stream/message
buffer work and CDC/MTP class processing. That path:

* trims time spent in IRQ context, and
* lets CDC/MTP class logic run at a normal task priority.

However, the task path had latent defects and was not usable in
practice. The goal was a full fix that makes ON functionally
equivalent to OFF, while keeping OFF (the production-validated path)
bit-for-bit unchanged. ON stays **disabled by default** until it is
bench-validated on target hardware.

## Branch / PR layout

Three independent pull requests, all targeting `master`:

| PR | Branch | Commit | Scope |
| -- | ------ | ------ | ----- |
| 1 | `async-usb-fix` | `[EGD-000] Disambiguate Audio/codec.hpp include in Profile.hpp` | Pre-existing master build break on rt1051+MTP |
| 2 | `async-usb-fix` | `[EGD-000] Bump usb_stack: safe ENABLE_USB_TASK path, document option` (+ submodule `[MOS-000]`) | The USB task fix itself |
| 3 | `docs/async-usb-test-plan` | `[EGD-000] Add regression test plan for ENABLE_USB_TASK=ON` | `doc/test-plan-async-usb.md` |

Ticket IDs `EGD-000` / `MOS-000` are placeholders.

> Note: PR 1 and PR 2 currently live on the same `async-usb-fix`
> branch; to raise them as two separate PRs the Profile.hpp commit
> should be moved onto its own branch (or cherry-picked) before push.

## The seven issues fixed in `third-party/usb_stack`

Submodule commit
`[MOS-000] Make USB_DEVICE_CONFIG_USE_TASK=1 path safe to enable`
(`ade6d28`, 8 files, +311 / -43).

### composite.c

1. **Build break - undefined symbol.** `composite_deinit` referenced
   an undefined `composite` (typo for `instance`), so the file failed
   to compile with `USE_TASK=1`.
2. **Deinit ordering / use-after-free.** Reordered teardown to:
   disable NVIC -> stop controller -> delete USB task -> tear down
   MTP/VCom -> tear down class. The previous order let the USB task
   wake on a freshly-deleted message queue and dereference released
   class buffers.
3. **Task configuration.** Named macros `USB_DEVICE_TASK_PRIORITY`
   (= `tskIDLE_PRIORITY`, same as every `sys::Service`, so the USB
   task cannot starve WorkerDesktop) and `USB_DEVICE_TASK_STACK_SIZE`
   (grown 3 KiB -> 4 KiB; the old value was on the edge with
   `USB_ENABLE_LOGS`). Task name shortened to `usb_device`.

### cdc/virtual_com.{c,h} + cdc/mudita_vcom_buffers.h (new)

4. **Context-neutral buffer wrappers.** New `mudita_vcom_buffers.h`
   wraps `xStreamBuffer{Send,Receive}{,FromISR}` and selects the ISR
   vs. task variant at compile time via `USB_DEVICE_CONFIG_USE_TASK`,
   so endpoint callbacks no longer hardcode the `FromISR` form.
5. **Atomicity of send/recv vs. completion callbacks.** The
   `IsBusy -> drain/reschedule -> Submit` sequences in `VirtualComSend`
   / `VirtualComRecv` must be atomic against `OnSendCompleted` /
   `OnRecvCompleted`. With `USE_TASK=0` those callbacks run in ISR and
   `taskENTER_CRITICAL` masks them (kept on the OFF path). With
   `USE_TASK=1` they run in a task where `taskENTER_CRITICAL` does not
   synchronise, so a new `serialMutex` is used. Each path takes and
   gives the mutex independently using a try-lock/bail pattern: the
   callback try-locks with timeout 0 and returns early if the caller
   holds the mutex, confident that the caller's in-flight transfer will
   trigger another completion event. `serialMutex` is allocated only
   when `USE_TASK=1`. `VirtualComDetached` / `VirtualComReset` also
   drain leftover bytes through the new wrappers.

### mtp/mtp.c + mtp/mudita_mtp_buffers.h (new)

6. **MTP buffer wrappers + task priority + latent bug.** New
   `mudita_mtp_buffers.h` provides the same wrapper idea for
   `xMessageBuffer*` and `xSemaphoreGive`. `OnConfigurationComplete`
   / `OnIncomingFrame` / `OnOutgoingFrameSent` switched to the
   wrappers. MTP task priority raised `tskIDLE_PRIORITY` ->
   `tskIDLE_PRIORITY+2`: `poll_new_data` shapes USB throughput and
   touches the filesystem, and on Idle it was starved by other
   Idle-level services during large transfers; it stays below
   `sys::ServicePriority::High` (3) used by SystemWatchdog. Also fixed
   a bogus second `outputBox = NULL` in `MtpDeinit` (should be
   `inputBox = NULL`). Task name shortened to `mtp`.

### cdc/usb_device_cdc_acm.h, mtp/usb_device_mtp.h

7. **`IsBusy` contract documentation.** Documented that
   `USB_DeviceClassCdcAcmIsBusy` / the MTP equivalent return an
   unguarded snapshot of an internal `uint8_t` and are only meaningful
   under the correct serialisation primitive (critical section vs.
   mutex) depending on `USB_DEVICE_CONFIG_USE_TASK`.

### cdc/virtual_com.c (follow-up fix)

8. **RX-path race in `OnRecvCompleted`.** `VirtualComRecv` held
   `serialMutex` to protect the `IsBusy → RescheduleRecv` sequence,
   but `OnRecvCompleted` never acquired it, leaving the RX path with
   the same race the TX path was fixed for. Both could call
   `RescheduleRecv` → `USB_DeviceCdcAcmRecv` concurrently; the second
   call returned `kStatus_USB_Busy` and the reschedule was silently
   missed. Fixed by adding the same try-lock/bail guard (timeout 0) to
   `OnRecvCompleted`, mirroring `OnSendCompleted`.

## Main repository changes

Commit `[EGD-000] Bump usb_stack: safe ENABLE_USB_TASK path, document option`
(`a20ed22ee`):

* `third-party/usb_stack` submodule pointer bumped to `ade6d28`.
* `products/PurePhone/config.cmake`, `products/BellHybrid/config.cmake`
  - documented the `ENABLE_USB_TASK` option: OFF is kept until
  bench-validated on hardware; BellHybrid does not enable MTP, so the
  toggle affects only CDC there.

## Separate prerequisite fix (Profile.hpp)

Commit `[EGD-000] Disambiguate Audio/codec.hpp include in Profile.hpp`
(`8be73eac5`) is an independent master build break uncovered while
linking the rt1051 + MTP build, kept as its own commit:

* `module-audio/Audio/Profiles/Profile.hpp` - the quoted include
  `"Audio/codec.hpp"` was ambiguous between
  `module-bsp/board/rt1051/bsp/Audio/codec.hpp` (earlier in the `-I`
  list, lacks the `audio::codec` namespace) and
  `module-audio/Audio/codec.hpp`. GCC picked the BSP header, breaking
  every TU that transitively pulls `Profile.hpp`. Switched to the
  project-rooted angle-bracket form `<module-audio/Audio/codec.hpp>`,
  matching the convention already used in `module-audio/Audio/codec.hpp`.

## Verification performed

* rt1051 RelWithDebInfo, PurePhone, MTP=ON: the four touched objects
  (`composite`, `virtual_com`, `usb_device_cdc_acm`, `mtp`) compile
  cleanly under `arm-none-eabi-gcc` with `-Werror`, for both
  `ENABLE_USB_TASK=ON` and `OFF`.
* Full link of `lib/libusb_stack.a` for rt1051 with ON succeeds (after
  the Profile.hpp fix).
* Note: the Linux simulator build does **not** compile
  `third-party/usb_stack` (it uses `board/linux/usb_cdc`), so this
  change is validated on rt1051 only.

## Status / not done

* Nothing pushed to origin (per request).
* `ENABLE_USB_TASK` stays OFF by default; functional equivalence ON vs
  OFF is validated per `doc/test-plan-async-usb.md`.
* Optional follow-up: BellHybrid rt1051 link verification (CDC only,
  no MTP).
