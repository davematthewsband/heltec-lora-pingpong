# Changelog

Notable local changes are recorded here. Plans belong in [the roadmap](docs/roadmap.md).

## Unreleased

### Firmware preparation

- Pinned the V3 build to Espressif32 6.6.0 and Heltec ESP32 Dev-Boards 2.1.7, replacing the mismatched saved dependencies and supplying explicit board/radio definitions.
- Updated MCU initialization for the Heltec API; bounded transmit formatting and rejected receive packets that cannot fit with a terminator.
- Added host receive-boundary checks with memory sanitizers. V3 compilation verified; no flashing or packet reception tested.

### Added

- Recovered PingPong OLED application and saved PlatformIO configuration for bidirectional LoRa/OLED experiments, imported without firmware edits.
- Project README, working agreement, staged roadmap, evidence notes and source provenance with import hashes.
- Ignore rules for generated builds, editor state, credentials and captures.
- Local Git repository on main, initialized on 15 November 2026.

### Changed

- Repurposed the repository around the recovered application after locating it in the user's PlatformIO projects.
- Preserved the older Heltec 1.1.0 library, examples, metadata, images, upstream README and license under archive/heltec-library-1.1.0/.

No firmware build, flashing or radio validation was performed during the original recovery. At recovery, saved dependencies, application license provenance and physical hardware remained unverified; subsequent build preparation is recorded above.

Archived OTA OLED example Wi-Fi values were replaced with placeholders before publication. An original copy was kept outside the repository in a private local temporary directory.
