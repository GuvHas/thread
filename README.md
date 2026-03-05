# XIAO ESP32C6 Matter-over-Thread (FTD) Example

This project is an ESP-IDF 5.5.2 + ESP-Matter template for **Seeed Studio XIAO ESP32C6** that:

- boots as a Matter accessory,
- advertises for commissioning (BLE + Matter QR/manual code),
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

This NodeLabel is what commissioners typically expose as a friendly device name and can also be changed later from Matter controllers.

## 4) Build + Flash + Monitor

```bash
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

(Use the correct serial port for your machine.)

## 5) Commission from phone and join existing Thread network

Use a Matter commissioner app (Apple Home / Google Home / SmartThings / CHIP Tool) that has access to your existing Thread credentials.

Typical flow:

1. Put device in commissioning mode (fresh flash does this automatically).
2. Scan Matter QR code or enter manual pairing code from monitor output.
3. Commissioner transfers dataset; device joins existing Thread network.

## Notes specific to this example

- Uses an **On/Off Light** endpoint as a simple Matter device type for bring-up.
- Thread is configured as **FTD** in `sdkconfig.defaults`.
- USB power only: no battery-centric power management enabled.

## Troubleshooting

- If commissioning fails, erase and retry:
  ```bash
  idf.py erase-flash flash monitor
  ```
- If OpenThread options look inconsistent, run `idf.py menuconfig` and verify Thread + Matter options are enabled.
- If your commissioner cannot discover the device, make sure BLE is enabled on phone and the app supports Matter commissioning.
