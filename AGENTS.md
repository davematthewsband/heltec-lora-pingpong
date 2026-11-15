# Working on Heltec ESP32 LoRa experiments

## Agree the change before editing

- Inspect relevant material read-only, explain the proposed change, scope and meaningful effects, then wait for the user to naturally agree or refine the proposal before changing files or configuration.
- Keep this conversational; do not introduce a formal approval ritual or repeat requests for agreement. Once a scope is agreed, complete routine work within it without asking again.
- Discuss material scope changes before implementing them. Do not commit, push, publish, flash firmware or change attached hardware unless included in the agreed scope.

## Project and learning priorities

- Read README.md, CHANGELOG.md, docs/roadmap.md, docs/provenance.md and relevant learnings before changing the project.
- The active project is recovered from PingPong OLED: a PlatformIO sketch with bidirectional LoRa and OLED output. The saved board target is heltec_wifi_lora_32_V3; physical hardware and previous success remain unverified. The older Heltec library is preserved under archive/heltec-library-1.1.0/.
- Preserve original source, examples, library metadata, upstream README and license notices. Do not describe the snapshot as pristine upstream: earlier edits cannot be established without history.
- Verify each board revision, radio variant and pin definitions before choosing dependencies or implementing hardware support. The saved V3 target is evidence of intended configuration, not proof of physical hardware or dependency compatibility.
- Work in small, explainable stages: identify hardware, reproduce a build, then verify the existing bidirectional exchange.
- Prefer understandable changes to selected sketches or clearly separated project firmware over broad rewrites of the bundled library. Introduce and pin a build stack only when the target is known.

## Development and hardware

- Use local files, terminal tools and APIs. Do not use Computer Use or ask the user to enable it.
- Preserve user files and local configuration. Keep credentials, private device configuration, captures, build output and firmware binaries out of version control; use placeholders for shared configuration.
- Check current official hardware and dependency documentation when making compatibility claims or implementing support. Historical upstream instructions are reference material, not a verified current build recipe.
- Confirm antennas, hardware frequency variants and applicable frequency/power/duty-cycle requirements before transmitting. Do not copy an example frequency or output power as an approved configuration.
- Record matching radio parameters for both peers. The archived OLED sender and receiver have different BAND values; do not confuse those examples with the active sketch, configured for 868 MHz.
- Treat RSSI as signal strength, not measured distance. A transmitted packet or successful compile does not establish reception.

## Changelog and validation

- Update CHANGELOG.md under Unreleased for notable implemented changes. Plans belong in docs/roadmap.md; evidence and uncertainties belong in docs/learnings.md.
- Reserve release versions/dates for an explicitly agreed release. Do not change upstream library version metadata just to mark personal project setup.
- Documentation-only work needs local link and whitespace checks, not firmware tests.
- For firmware changes, compile for the verified target and test pure protocol/timing logic where useful. Do not invent board identifiers, successful test results or historical milestones.
- Distinguish documentation checks, compilation, flashing, serial output, actual packet reception and bidirectional reliability. Record limitations and hardware checks still required.
- Review changes for secrets, generated files and unrelated edits before reporting the result.
