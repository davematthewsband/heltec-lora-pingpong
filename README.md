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

The recovered sketch now compiles for the user-confirmed V3 target with PlatformIO Espressif32 **6.6.0** (Arduino **2.0.14**) and Heltec ESP32 Dev-Boards **2.1.7**. The unused dependency list and incompatible Sandeep Mistry LoRa dependency have been replaced by the Heltec library used by this sketch.

The configuration explicitly selects Heltec board 30, SX1262, the internal slow clock and EU868 definitions. Radio/OLED pins use the V3 definitions: NSS 8, clock 9, MOSI 10, MISO 11, reset 12, busy 13, DIO1 14; OLED SDA 17, SCL 18, reset 21 and Vext 36. A GPIO count alias accommodates the Heltec radio C source's IDF header inclusion.

```sh
pio run -e heltec_wifi_lora_32_V3
python3 tests/test_receive.py
```

The host test requires Python 3 and a C++ compiler with address/undefined sanitizers. It exercises the actual receive callback at packet-size boundaries. Packets of 30 bytes or more are rejected; up to 29 bytes plus the string terminator fit. Transmit formatting is also bounded.

**Build verified; hardware operation unverified.** Before uploading, confirm the radio frequency variant, suitable antenna, local frequency/power/duty-cycle requirements and agreed hardware-test scope. The saved 868 MHz / 10 dBm / one-second reply delay is retained, not approved for transmission. The sketch starts transmitting after boot. Both peers need matching radio settings. No flashing or radio testing has been performed in this preparation stage.

Official references: [PlatformIO V3 target](https://docs.platformio.org/en/latest/boards/espressif32/heltec_wifi_lora_32_V3.html), [Heltec V3 hardware](https://heltec.org/project/wifi-lora-32-v3/), [Heltec library](https://github.com/HelTecAutomation/Heltec_ESP32).

## Repository layout

- [Recovered firmware](src/main.cpp) and [saved PlatformIO configuration](platformio.ini).
- [Working agreement](AGENTS.md), [changelog](CHANGELOG.md), [roadmap](docs/roadmap.md) and [learnings](docs/learnings.md).
- [Source provenance](docs/provenance.md).
- [Archived Heltec library](archive/heltec-library-1.1.0/README.upstream.md): the older library collection originally in this folder. It is retained for reference outside the active firmware source tree.

Build output, editor state, credentials and diagnostic captures are excluded from Git. The original PlatformIO project folder is unchanged.

## Attribution and license

The archived Heltec library carries the [MIT license](archive/heltec-library-1.1.0/LICENSE), also retained at the repository root. The recovered application uses Heltec APIs and appears related to their examples, but its exact upstream source and applicable application license have not been established. The archived library's license is not evidence of a separate license grant for the recovered application. See [provenance](docs/provenance.md).
