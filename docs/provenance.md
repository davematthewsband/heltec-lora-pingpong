# Source provenance

## Recovered application

Imported on 15 November 2026 from the user's local `PlatformIO/Projects/PingPong OLED` folder. Only `src/main.cpp` and `platformio.ini` were imported. Generated `.pio` contents, editor state and template-only directories were excluded. The source folder was left unchanged.

SHA-256 hashes at import:

- `src/main.cpp`: `07d2ef23a01de70e1ac7067e6d2a74b847a7be449f7ad124a8596f0fc5f4107c`
- `platformio.ini`: `dbfc5b3c56412a486def2fd391241d881db68cb89a99d52ff0bc350fabc4d65b`

The application includes Heltec headers and contains example-like code. Its exact upstream source, revision and application license have not been established. No new authorship or license claim is made for it.

## Archived library

The pre-existing library is preserved under `archive/heltec-library-1.1.0/`, including original source, examples, metadata, images, upstream README and MIT license with Heltec Automation attribution. Earlier local modifications cannot be established because no prior Git history was available.

The root LICENSE duplicates the archived library's license. Its existence does not establish the provenance or license of the separately recovered application.

Archived OTA OLED example Wi-Fi values were replaced with placeholders before publication. An original copy was kept outside the repository in a private local temporary directory.

## Subsequent local firmware preparation

The import hashes above describe the recovered baseline, not the current edited files. On 15 November 2026 the dependency configuration, MCU initialization and packet bounds were repaired locally; the baseline remains in Git history. The archived library was not modified. These repairs do not resolve the application's original upstream provenance/license uncertainty.

## Shared demo development

On 15 November 2026 the user identified the unchanged local Arduino sketches `Finn_lora_Bat/Finn_lora_Bat.ino` and `Anny_Lora_Bat/Anny_Lora_Bat.ino` as their older device firmware. These supplied behavioural context (labels, battery display, radio exchange and sleep), not proof of the current hardware image.

Before implementing the agreed shared button/light demo, the current locally repaired PingPong source was preserved as `archive/pingpong-oled-baseline.cpp`; its historical receive test now reads that file. The active firmware and separate protocol/button logic implement the new demo. Original Arduino projects and archived Heltec library files were not edited. Original application provenance/license uncertainty remains unresolved.
