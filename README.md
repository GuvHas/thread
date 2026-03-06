# XIAO ESP32C6 Matter-over-Thread (FTD) Example

This project is an ESP-IDF 5.5.2 + ESP-Matter template for **Seeed Studio XIAO ESP32C6** that:

- boots as a Matter accessory,
- advertises for commissioning over BLE,
- can be commissioned by a phone,
- joins an **existing Thread network** as an **FTD** (Full Thread Device),
- is intended to run from normal **USB power**.

## 1) Prerequisites

Follow Seeed's setup flow first (toolchain + ESP-IDF + Matter environment):
<https://wiki.seeedstudio.com/xiao_esp32_matter_env/>

Then ensure:

- ESP-IDF checked out at `v5.5.2`
- `idf.py --version` reports 5.5.2
- Python environment has `esp-matter` dependencies via component manager

## 2) Open in VS Code

1. Install the **Espressif IDF** extension in VS Code.
2. `File -> Open Folder` on this repo.
3. Set target: `ESP32-C6`.
4. Ensure the extension uses your ESP-IDF 5.5.2 environment.

## 3) Configure

```bash
idf.py set-target esp32c6
idf.py menuconfig
```

Optional rename before first commissioning:

- `Component config -> Matter device options -> Default Matter Node Label`
- default is `XIAO-ESP32C6`

This NodeLabel is the friendly device name commissioners usually show. You can also rename later from most Matter controller apps.

## 4) Build + Flash + Monitor

```bash
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

(Use the correct serial port for your machine.)

## 5) How to actually get the device to join your Thread network

The ESP32-C6 does **not** invent a Thread network by itself in this app. A commissioner (phone app or chip-tool) must provide Thread credentials from an existing Thread Border Router network.

### Option A: Phone app (recommended)

Use Apple Home / Google Home / SmartThings (or another Matter commissioner) on a phone that is already in the same home/fabric with a Thread Border Router.

1. Flash and reboot the device.
2. In monitor logs, find the Matter onboarding payload (QR/manual pairing code).
3. In your phone app, choose **Add Matter Device** and scan/enter that code.
4. Keep phone BLE enabled and near the board during setup.
5. The app commissions over BLE, sends operational credentials + Thread dataset.
6. Device attaches to Thread and appears in the app.

If this succeeds, your code is fine; the network join is driven by the commissioner.

### Option B: `chip-tool` (developer flow)

If you want explicit CLI control, use `chip-tool` from a machine that has access to your Matter fabric.

1. Put device in commissioning mode (fresh flash usually does this automatically).
2. Use the setup payload code from serial logs.
3. Commission to your fabric using BLE rendezvous:

```bash
chip-tool pairing ble-thread <node-id> hex:<THREAD_ACTIVE_DATASET_HEX> <setup-pin-code> <discriminator>
```

Where:

- `<THREAD_ACTIVE_DATASET_HEX>` comes from your border router network,
- `<setup-pin-code>` and `<discriminator>` come from the device onboarding payload/logs.

After successful pairing, verify with:

```bash
chip-tool descriptor read device-type-list <node-id> 0
chip-tool onoff read on-off <node-id> 1
```

## 6) Notes specific to this example

- Uses an **On/Off Light** endpoint as a simple Matter device type for bring-up.
- Thread is configured as **FTD** in `sdkconfig.defaults`.
- USB power only: no battery-centric power management enabled.

## 7) Troubleshooting commissioning/join

- If pairing repeatedly fails, erase and retry:
  ```bash
  idf.py erase-flash flash monitor
  ```
- Ensure your phone commissioner can access both BLE and the target Matter fabric.
- Ensure your Thread Border Router is online and already part of that same home ecosystem.
- If using `chip-tool`, make sure the active dataset is valid and from the exact Thread network you expect.
- If discovery works but join fails, monitor logs for commissioning timeout/invalid setup code errors.


## 8) Windows fix for `FileNotFoundError` while downloading `esp_matter`

If VS Code fails during `idf.py set-target esp32c6` with a path like:

- `...\Espressif\ComponentManager\Cache\...\connectedhomeip...\BooleanStateConfigurationTestEventTriggerHandler.cpp`

this is almost always a **Windows long-path limitation** during Component Manager unzip.

### Fix steps (Windows)

1. **Enable Win32 long paths** (required):
   - `gpedit.msc` -> `Computer Configuration -> Administrative Templates -> System -> Filesystem -> Enable Win32 long paths = Enabled`
   - or registry (`Run as Administrator`):

   ```powershell
   reg add HKLM\SYSTEM\CurrentControlSet\Control\FileSystem /v LongPathsEnabled /t REG_DWORD /d 1 /f
   ```

2. **Reboot Windows**.

3. **Use short working paths** (recommended):
   - move project to something short like `C:\ws\thread`
   - keep ESP-IDF at short path like `C:\esp\v5.5.2\esp-idf`

4. **Clear partially-downloaded component cache** and retry:

   ```powershell
   rmdir /s /q %LOCALAPPDATA%\Espressif\ComponentManager\Cache
   ```

5. Reopen VS Code from **ESP-IDF PowerShell** and run again:

   ```powershell
   idf.py fullclean
   idf.py set-target esp32c6
   idf.py build
   ```

### Why this happens

`esp_matter` contains nested `connectedhomeip` paths with long filenames. If Windows long paths are disabled (or not yet applied after enabling), Python unzip fails with exactly the `FileNotFoundError` you posted.

