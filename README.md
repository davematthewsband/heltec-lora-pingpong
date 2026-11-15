# Heltec LoRa button and light demo

One firmware for two Heltec WiFi LoRa 32 **V3** devices: press either device's PRG button to toggle the other device's white onboard LED. Each device identifies itself automatically and shows battery and peer status on its OLED.

## Using the demo

- Wake/power both devices. Each listens immediately and sends a discovery/status packet after a random 0.5–2.5-second delay, then every 15–20 seconds. The first compatible peer heard is selected until restart/wake. Both devices use exactly the same image; there are no sender/receiver builds.
- **Short PRG press:** toggle the peer's LED. The sender shows confirmation only after a matching acknowledgement; retries reuse the command ID so they cannot toggle the light twice.
- **Hold PRG for two seconds:** ask the peer to sleep, then sleep locally. Release the button to allow local sleep. The peer acknowledges and stays awake for a ten-second retry window before sleeping. The sender sleeps about one second after acknowledgement. If there is no acknowledgement after three attempts, it shows an unconfirmed result and sleeps anyway; the peer may still be awake. With no discovered peer, it shows the failure and sleeps locally.
- **Press PRG while asleep:** wake that device and resume listening. Release it before making a new command; the wake press is consumed. Wake the second device with its own button too—LoRa cannot wake this sleeping firmware.
- Devices stay awake until a sleep command; there is no automatic timer wake. Sleeping turns the screen and white LED off. This is deep sleep, not electrical power-off; a charge/power indicator can remain lit.

The OLED shows a short device ID, own battery voltage/estimated percentage, peer battery, RSSI and age of last received packet, both LED states, and command result. Peer values are last reported values, not live readings. RSSI is signal strength, not distance. A peer cannot be declared absent immediately: use packet age and command acknowledgement. Local LED state resets to off on wake/restart.

Battery sensing uses the **original V3** active-low GPIO37 switch, GPIO1 ADC and 390k/100k divider. Readings are averaged using the ADC millivolt API. Percentage is a rough linear estimate between 3.2 V and 4.2 V, clamped to 0–100%; readings outside 2.5–4.5 V show `--`. This does not reliably detect an absent battery or measure charge state while charging. Later V3 revisions need separate verification; do not infer compatibility from the generic V3 build target.

## Build and checks

Pinned stack: PlatformIO Espressif32 **6.6.0**, Arduino **2.0.14**, Heltec ESP32 Dev-Boards **2.1.7**, board `heltec_wifi_lora_32_V3`. Board/radio definitions select ESP32-S3 and SX1262; the library is unchanged.

```sh
pio run -e heltec_wifi_lora_32_V3
python3 tests/test_demo.py
python3 tests/test_receive.py
```

Host checks need Python 3 and a C++ compiler with address/undefined sanitizers. Demo tests exercise wire validation, button debounce/wake suppression, discovery, acknowledgements, command retries/duplicates, simultaneous commands, sleep and timer rollover. The older receive-boundary tests now target the preserved PingPong baseline.

**No flashing or hardware validation is included in this change.** Confirm both radio variants, suitable antennas and applicable frequency/power/duty-cycle requirements before an agreed upload/test. The firmware will transmit discovery packets after boot. It retains 868 MHz, 10 dBm, 125 kHz bandwidth, SF7, coding rate 4/5, eight-symbol preamble, CRC on and normal IQ. Both peers must match. The retry/status timing is not a regulatory duty-cycle limiter; rapid button use also generates radio traffic.

Hardware checks still needed: actual reception and acknowledgement in both directions, simultaneous startup/presses, LED and OLED output, battery voltage against a meter, lost-peer reporting, long-press sleep on both devices, PRG wake independently, and battery-powered sleep current. A compile or transmit log establishes none of these.

## Project layout and history

- [Shared firmware](src/main.cpp), [protocol/button logic](include/demo_protocol.h), [PlatformIO configuration](platformio.ini) and [protocol notes](docs/demo-protocol.md).
- [Preserved PingPong baseline](archive/pingpong-oled-baseline.cpp), including the earlier local build/bounds repairs.
- [Working agreement](AGENTS.md), [changelog](CHANGELOG.md), [roadmap](docs/roadmap.md), [learnings](docs/learnings.md), and [source provenance](docs/provenance.md).
- [Archived Heltec library](archive/heltec-library-1.1.0/README.upstream.md), retained outside the active firmware tree with its metadata, examples and notices.

The old Finn/Anny Arduino sketches remain unchanged in the user's Arduino folder. Build output, editor state, device captures and private configuration are excluded from Git.

## Attribution and license

The archived Heltec library carries the [MIT license](archive/heltec-library-1.1.0/LICENSE), also retained at the root. The recovered application uses Heltec APIs and appears related to their examples, but its original upstream source and applicable license remain uncertain. The archived library's license does not establish a separate license grant for that application; see [provenance](docs/provenance.md).
