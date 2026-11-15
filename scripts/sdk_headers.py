"""Expose SDK GPIO declarations under an unambiguous include path.

Heltec also supplies driver/gpio.h. Adding the SDK driver's parent directory
lets application code include include/driver/gpio.h without shadowing Heltec.
"""
from pathlib import Path
Import('env')
framework = Path(env.PioPlatform().get_package_dir('framework-arduinoespressif32'))
env.Append(CPPPATH=[str(framework / 'tools/sdk/esp32s3/include/driver')])
