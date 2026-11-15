# Roadmap

## 1. Confirm the recovered baseline

- Identify both physical boards, radio variants and antennas.
- Establish whether PingPong OLED was used on both devices and what previously worked.
- Trace the recovered application to its upstream example and establish its license.

Acceptance: record confirmed facts and retain unknowns explicitly.

## 2. Reproduce a clean build

- Completed for the user-confirmed V3 target: dependency reconciliation, pinned stack and successful compilation. See learnings for versions and limitations.
- Confirm the physical frequency variant and antenna before any hardware test.
- Completed: bounded receive/transmit handling and host receive-boundary checks.

Acceptance: a clean build for the confirmed hardware; any firmware repair is discussed before edits.

## 3. Validate the two-device exchange

- Agree shared radio parameters and hardware-test scope before flashing/transmitting.
- Observe fresh messages in both directions, OLED output, startup contention, peer loss and button behavior.
- Record packet counts, duration, environment and limitations.

Acceptance: evidence of actual bidirectional reception rather than transmission logs alone.

## 4. Improve reliability if needed

Discuss startup roles/random backoff, bounded packet parsing, acknowledgements and retry behavior based on measured results. Add meaningful tests for introduced protocol logic.

## 5. Shared button and light demo

Agreed scope: same V3 firmware on two devices, automatic IDs, acknowledged peer LED toggle on short PRG press, peer sleep followed by local sleep on long press, independent PRG wake and battery/OLED status.

Implemented locally; build and host validation are recorded in learnings. Next, agree hardware testing after confirming each radio variant and antenna. Check short presses in both directions, lost ACK/peer cases, simultaneous operation, peer sleep plus local sleep and independent wake. Measure battery voltage and sleep current rather than assuming them from compilation.

Friendly names, persistent pairing, more than two peers and automatic periodic sleep remain future discussion items.
