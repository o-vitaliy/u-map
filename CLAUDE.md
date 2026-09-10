# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project state

This is a PlatformIO/Arduino firmware project targeting the ESP32-C3 (`esp32-c3-devkitc-02` board, `espressif32` platform, `arduino` framework — see [platformio.ini](platformio.ini)). The project currently contains only the default PlatformIO scaffold: [src/main.cpp](src/main.cpp) has empty `setup()`/`loop()` and a placeholder `myFunction`. `include/`, `lib/`, and `test/` are empty aside from PlatformIO's template README files. There is no application logic yet.

## Commands

Build, upload, and monitor via the PlatformIO CLI (`pio`):

```bash
pio run                       # build
pio run --target upload       # build and flash to the connected board
pio device monitor             # serial monitor
pio run --target uploadfs     # upload filesystem image (if/when one is added)
pio test                      # run unit tests (test/ is currently empty)
pio check                     # static analysis
```

The environment name is `esp32-c3-devkitc-02`; commands operate on it by default since it's the only env in `platformio.ini`. To target it explicitly: `pio run -e esp32-c3-devkitc-02`.
