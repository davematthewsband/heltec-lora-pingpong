# Heltec LoRa PingPong

Recovered ESP32 LoRa experiment for two devices exchanging messages and displaying reception details on their OLED screens.

## Firmware behavior

The same recovered sketch contains both transmit and receive logic:

1. Starts in transmit mode and sends `hello <counter>,Rssi:<last received RSSI>`.
2. Switches to receive mode after transmission completes.
3. On reception, records the payload and RSSI, then schedules another transmission after a one-second delay.
4. Displays received data, packet size, RSSI and the send counter on the OLED.

The sketch also contains button-triggered resend/deep-sleep behavior. Both peers starting together can contend; this is an experiment, with no recorded reliability guarantee.

## Recovered configuration

| Setting | Value in the saved project |
| --- | --- |
| PlatformIO environment / board | `heltec_wifi_lora_32_V3` |
| Framework | Arduino |
| Frequency | 868 MHz |
| Transmit power | 10 dBm |
| Bandwidth | 125 kHz |
| Spreading factor | 7 |
| Coding rate | 4/5 |
| Preamble | 8 symbols |
| Serial baud rate | 115200 |

These describe the source configuration. Physical board identities, past successful operation and current build compatibility remain unverified. Confirm each board's radio variant, antenna and suitable transmission settings before a hardware test.

## Build status

`src/main.cpp` and `platformio.ini` were copied byte-for-byte from the local **PingPong OLED** PlatformIO project. No firmware changes or flashing were performed during recovery.

The saved configuration lists `sandeepmistry/LoRa` under `lib_deps`, while the sketch includes `LoRaWan_APP.h` and `HT_SSD1306Wire.h` and uses Heltec's `Radio` API. Heltec dependencies appear separately under `lib_deps_all`. This dependency setup has not been validated; a clean build is still required. Platform/toolchain versions are not pinned.

After resolving the dependencies for the verified hardware, the intended build entry point is:

```sh
pio run -e heltec_wifi_lora_32_V3
```

This is not yet a verified build recipe. Upload instructions will be added after the build and hardware are confirmed.

## Repository layout

- [Recovered firmware](src/main.cpp) and [saved PlatformIO configuration](platformio.ini).
- [Working agreement](AGENTS.md), [changelog](CHANGELOG.md), [roadmap](docs/roadmap.md) and [learnings](docs/learnings.md).
- [Source provenance](docs/provenance.md).
- [Archived Heltec library](archive/heltec-library-1.1.0/README.upstream.md): the older library collection originally in this folder. It is retained for reference outside the active firmware source tree.

Build output, editor state, credentials and diagnostic captures are excluded from Git. The original PlatformIO project folder is unchanged.

## Attribution and license

The archived Heltec library carries the [MIT license](archive/heltec-library-1.1.0/LICENSE), also retained at the repository root. The recovered application uses Heltec APIs and appears related to their examples, but its exact upstream source and applicable application license have not been established. The archived library's license is not evidence of a separate license grant for the recovered application. See [provenance](docs/provenance.md).
