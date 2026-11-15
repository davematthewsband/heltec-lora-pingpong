# Learnings and evidence

## 15 November 2026 — locating the experiment

The initial folder contained an older Heltec Arduino library with metadata version 1.1.0 and no Git history. Its examples did not establish which personal experiment had been run.

The user then pointed to their PlatformIO projects. **PingPong OLED** contains the likely experiment: one sketch alternates transmission and reception, sends `hello` counters and RSSI, and displays received data on the OLED. The user selected this as the basis for the repository.

### Confirmed from saved files

- PlatformIO board/environment: `heltec_wifi_lora_32_V3`; Arduino framework.
- Radio configuration: 868 MHz, 10 dBm, 125 kHz bandwidth, SF7, coding rate 4/5 and eight-symbol preamble.
- Transmit completion selects receive mode; receiving selects transmit mode.
- The saved dependency lists and application includes need reconciliation before claiming a reproducible build.
- Application and PlatformIO configuration were copied byte-for-byte; original files remain unchanged.
- Old library contents and license were moved intact into `archive/heltec-library-1.1.0/` to keep them outside the active source tree.
- Incoming payload handling copies an unchecked size into a 30-byte buffer and writes a terminator at that size. This is an observed issue to address in a separately agreed firmware repair.

### Still unknown

Physical board identities, previous build/toolchain versions, past hardware success, application upstream revision/license and current clean-build compatibility. No firmware compilation, flashing or radio testing was performed during repository recovery.

## Future test records

Record date, board identities, source revision, toolchain, radio parameters, antennas, power source, commands/settings and actual packet evidence. Separate observations from suspected causes.

Archived OTA OLED example Wi-Fi values were replaced with placeholders before publication. An original copy was kept outside the repository in a private local temporary directory.

## 15 November 2026 — V3 build preparation

- User confirmed “v3”; frequency variant, antennas and peer count remain unconfirmed.
- Build: PlatformIO Espressif32 6.6.0, Arduino 2.0.14, Heltec ESP32 Dev-Boards 2.1.7, ESP32-S3 toolchain 8.4.0+2021r2-patch5. Explicit SX1262 / WIFI_LORA_32_V3 / HELTEC_BOARD=30 / SLOW_CLK_TPYE=0 definitions select the V3 radio stack. EU868 definitions match the retained source frequency but do not establish permission to transmit.
- Heltec's MCU API requires board and clock arguments. Its radio C source needs a GPIO_PIN_COUNT alias to SOC_GPIO_PIN_COUNT with this SDK header combination. Dependency sources were not edited.
- Receive callback rejects sizes >= 30 before copying; normal reception and return to RX after oversized packets passed eight host boundary cases (0, 1, 28, 29, 30, 31, 255, 256) with address/undefined sanitizers.
- Firmware compiled and linked successfully. Initial verified image: 21,448 bytes RAM, 337,249 bytes flash. Dependency/SDK warnings remain, including a GPIO_PIN_COUNT macro redefinition (both definitions resolve to SOC_GPIO_PIN_COUNT), sensor-library warnings and an SDK UART return warning; a successful build does not validate hardware.
- No upload, serial observation, OLED verification or actual reception was performed. Saved transmit settings and the one-second reply delay still require regulatory review before radio use.
