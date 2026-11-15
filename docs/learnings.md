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
