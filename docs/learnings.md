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

## 15 November 2026 — shared button/light demo

- User identified the local Finn/Anny Arduino sketches as the previously flashed applications, then confirmed the boards are original **V3 exactly**. Those sketches remain unchanged. The older labels and voltage display are source evidence; no current flashed image was inspected.
- Agreed behaviour: identical firmware, automatic IDs, short PRG press toggles the peer LED, long press asks the peer to sleep then sleeps locally, separate PRG presses wake each device. No automatic timed sleep/wake.
- Verified original V3 schematic: GPIO0 PRG switch to ground with pull-up; GPIO35 white LED active high; GPIO37 controls the battery P-channel switch active low; GPIO1 senses a 390k/100k divider. See official links in [protocol notes](demo-protocol.md). Later V3.2 battery examples use a different polarity and were not copied. Original V3 voltage uses averaged ADC millivolts × 4.9; calibration, absent-battery behaviour and percentage accuracy remain unmeasured.
- Preserved the previous locally repaired active source as `archive/pingpong-oled-baseline.cpp`; its eight receive-boundary sanitizer checks still pass. Archived Heltec library and original Arduino sketches were not edited.
- New host checks pass with address/undefined sanitizers and compiler warnings treated as errors: packet length/header validation, IDs, battery bounds, button debounce/boot hold/short/long press, peer selection, duplicate and older command suppression, lost/mismatched ACKs, commands in both directions, simultaneous commands, sleep grace/release/idle gating, failed/absent peer and timer rollover.
- V3 firmware compiled/linked successfully with the existing pinned stack: **21,832 bytes RAM, 352,197 bytes flash**. Existing dependency/SDK warnings remain. Heltec's `driver/gpio.h` shadows the SDK header, so a small project build script exposes the SDK header through an unambiguous include path; no installed dependency files were edited.
- Radio parameters remain the prior 868 MHz / 10 dBm / SF7 / 125 kHz / 4/5 / preamble 8. Discovery/status and command retries use randomised timing, not a regulatory duty-cycle limiter. Frequency variant, antennas and applicable transmission settings still need confirmation before an agreed hardware test.
- No upload, serial observation, OLED/LED verification, physical packet reception, battery measurement, button wake test or sleep-current measurement was performed. Sleep requests use best-effort acknowledgement/retries: local sleep can occur without peer confirmation; sleeping devices need their own button press to wake.
