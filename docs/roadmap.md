# Roadmap

## 1. Confirm the recovered baseline

- Identify both physical boards, radio variants and antennas.
- Establish whether PingPong OLED was used on both devices and what previously worked.
- Trace the recovered application to its upstream example and establish its license.

Acceptance: record confirmed facts and retain unknowns explicitly.

## 2. Reproduce a clean build

- Verify official board/library documentation and resolve the saved dependency configuration.
- Pin a compatible platform and library stack; document exact build commands and results.
- Review the existing receive buffer handling, which currently copies unbounded received sizes into a 30-byte array, before hardware testing.

Acceptance: a clean build for the confirmed hardware; any firmware repair is discussed before edits.

## 3. Validate the two-device exchange

- Agree shared radio parameters and hardware-test scope before flashing/transmitting.
- Observe fresh messages in both directions, OLED output, startup contention, peer loss and button behavior.
- Record packet counts, duration, environment and limitations.

Acceptance: evidence of actual bidirectional reception rather than transmission logs alone.

## 4. Improve reliability if needed

Discuss startup roles/random backoff, bounded packet parsing, acknowledgements and retry behavior based on measured results. Add meaningful tests for introduced protocol logic.
