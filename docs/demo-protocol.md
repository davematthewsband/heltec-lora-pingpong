# Two-device demo protocol

Version 1 is a local two-device experiment, incompatible with the original Finn/Anny text packets. There is no permanent primary/secondary role: both listen and send occasional status packets with independently randomised timing. The first compatible status source selects the peer for that awake session. Use just two devices; there is no user pairing UI, authentication, encryption or mesh routing. Nearby compatible demo firmware can pair with or control a device. Full 48-bit chip identities address packets; only their last six hex digits are displayed.

## Wire format

Exactly 32 bytes; multi-byte integers are little-endian, explicitly encoded (no C struct padding).

| Offset | Bytes | Meaning |
| --- | --- | --- |
| 0 | 2 | Magic `FD` |
| 2 | 1 | Version 1 |
| 3 | 1 | Type: status 1, toggle 2, sleep 3, toggle ACK 4, sleep ACK 5 |
| 4 | 8 | Source chip ID (48 significant bits) |
| 12 | 8 | Target chip ID; zero allowed only for status |
| 20 | 4 | Boot nonce, or echoed command nonce in an ACK |
| 24 | 4 | Command sequence, echoed in an ACK |
| 28 | 2 | Battery millivolts; zero means unavailable |
| 30 | 1 | Source LED state, 0 or 1 |
| 31 | 1 | Reserved zero |

The parser rejects incorrect length, magic, version, type, reserved values, invalid IDs and broadcast commands. Address/peer filtering occurs before command processing. Radio CRC is enabled; this is error checking, not authentication.

## Commands and retries

A short press sends a toggle with a new local sequence number and boot nonce. One local command may be pending; additional short presses are ignored while it is pending. Long press supersedes an outstanding toggle with sleep. The receiver tracks its peer's latest command nonce/sequence; retrying the same command does not repeat its action. A matching ACK must echo the pending command's nonce, sequence and type. New boots select a fresh random nonce; state is not persisted in flash.

Transport prioritises ACKs, then commands, then status. It tries a command at most three times, with 2.5–3.25 seconds between attempts. Independent random jitter helps recover collisions, including simultaneous button presses. Status packets start after 0.5–2.5 seconds and recur after 15–20 seconds without a transmission. They are broadcast until a peer is selected, then addressed to that peer. These timings are collision mitigation, not guaranteed delivery or a duty-cycle enforcement mechanism.

On receiving sleep, a device cancels its pending command, turns its LED off, acknowledges and allows ten seconds for duplicate requests/ACK retries. The requester sleeps one second after a matching ACK, or after exhausting retries and displaying an unconfirmed result for 1.5 seconds. Both require button release stable for 100 ms, no outstanding ACK and idle radio. No finite radio handshake can guarantee both devices sleep together under arbitrary packet loss. The peer can sleep even if all its ACKs are lost; a peer missing every request remains awake. There is no remote wake.

## Button and power

PRG is GPIO0, active low; the white LED is GPIO35, active high. Button debounce is 35 ms and long press is two seconds. Startup/wake consumes the initial press until a stable release. Deep sleep uses GPIO0 EXT0 low-level wake with pull-up; the pad is returned to digital GPIO on wake. The firmware waits for release before sleep to prevent immediate wake from the held button. OLED/Vext and battery sensing are disabled, radio enters sleep, and output holds retain the power controls. Actual power consumption is unmeasured.

Original V3 battery sensing: GPIO37 low enables the P-channel switch; GPIO1 reads a 390k/100k divider, so ADC millivolts are multiplied by 4.9. Eight samples are averaged after a 10 ms settling delay. GPIO37 returns high after sampling. Verify accuracy against a meter; the old raw-ADC constant is not reused.

Official references checked on 15 November 2026:

- [Original V3 schematic](https://resource.heltec.cn/download/WiFi_LoRa_32_V3/HTIT-WB32LA%28F%29_V3_Schematic_Diagram.pdf): USER_SW, LED and battery switching/divider circuit.
- [ESP-IDF 4.4.7 ESP32-S3 sleep documentation](https://docs.espressif.com/projects/esp-idf/en/v4.4.7/esp32s3/api-reference/system/sleep_modes.html): EXT0 wake, pull configuration and RTC pad deinitialisation.
- [PlatformIO V3 target](https://docs.platformio.org/en/latest/boards/espressif32/heltec_wifi_lora_32_V3.html); installed Arduino variant definitions agree on GPIO35 LED and GPIO36 Vext.

The V3.2 official battery example uses a different enable polarity; the user explicitly confirmed original V3 for this implementation. Do not substitute newer examples without checking the matching schematic.
