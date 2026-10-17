# Voice Circular Display for ESP-IDF

Circular display UI framework (240x240) for real-time voice, speech synthesis (IPA/Formant), and audio visualization.

## Features

- **Voice & TTS Visualization**: Live phoneme display, IPA/word tracking with scrolling, spectrum analyzer / VU-meter
- **UTF-8/IPA Support**: Full Unicode rendering for proper phonetic symbols (ə, ʃ, θ, ˈ, etc.)
- **Widgets**: Reusable curved text rendering, spectrum analyzer, HUD overlays
- **Hardware**: Optimized for GC9A01 (240x240 round display), adaptable to other circular displays
- **Performance**: PSRAM sprite buffering, smooth animations

## Hardware Support

- **Display Controller**: GC9A01 (primary), adaptable to other round displays
- **Platform**: ESP32-P4 (recommended), ESP32-S3
- **Interface**: SPI (MIPI DSI support planned)
- **Resolution**: 240x240 circular

## Installation

### Using ESP-IDF Component Manager

Add to your project's `idf_component.yml`:

```yaml
dependencies:
  Strg-Alt-Entf-0x00/esp-idf-voice-circular-display:
    version: "^0.1.0"
```

### Manual Installation

```bash
cd components
git clone https://github.com/Strg-Alt-Entf-0x00/esp-idf-voice-circular-display.git
```

## Quick Start

```cpp
#include "formant_tts_display.h"

void app_main() {
    // Configure display hardware
    formant_display_config_t config = {
        .spi_host = SPI2_HOST,
        .pin_mosi = 28,
        .pin_sclk = 27,
        .pin_cs = 24,
        .pin_dc = 25,
        .pin_rst = 26,
        .pin_bkl = -1,
    };
    
    // Initialize display
    formant_display_init(&config);
    
    // Feed live TTS data
    formant_display_set_tts_data(
        0.7f,              // amplitude
        "Hello",           // current word
        "HH",              // current phoneme
        "həˈloʊ",         // IPA string
        0,                 // word index
        0                  // IPA index
    );
    
    // Render frame
    formant_display_render();
}
```

## Visual Layout

### TTS Active Mode
```
    Outer Ring: A B C D ... Z (alphabet, highlights active letter)
    
    Top Arc (68px):    IPA string "həˈloʊ" (scrolls during speech)
    Spectrum (20-60px): 32 rainbow bars showing audio amplitude
    Center:            Current phoneme "HH"
    Spectrum continues
    Bottom Arc (68px): Word string "Hello" (scrolls during speech)
```

### TTS Idle Mode
```
    Idle arcs show customizable text on top and bottom curves.
```

## Configuration

Run `idf.py menuconfig` and navigate to "Formant TTS Circular Display":

- Display resolution (default 240x240)
- Enable/disable TTS visualization
- Enable/disable alphabet ring
- Number of spectrum bars
- PSRAM usage
- **UTF-8/IPA font support** (requires font generation - see below)

### UTF-8 and IPA Character Support

For proper rendering of IPA phonetic symbols (ə, ʃ, θ, ˈ, etc.), you need to generate IPA-capable fonts:

```bash
# On Windows (via WSL):
cd tools
generate_fonts_windows.bat

# On Linux/Mac:
cd tools
./setup_and_generate_fonts.sh
```

This creates fonts in `include/fonts/` with full IPA support (~75KB Flash for 10pt, 12pt, 16pt).

Pre-rendered font headers are already included in the repository. See [include/fonts/README.md](include/fonts/README.md) for complete font generation documentation.

## API Reference

See [include/formant_tts_display.h](include/formant_tts_display.h) for full API documentation.

### Core Functions

- `formant_display_init()` - Initialize display hardware
- `formant_display_render()` - Render current frame
- `formant_display_set_tts_data()` - Feed live TTS speech data
- `formant_display_set_idle_text()` - Set idle/silence arc texts
- `formant_display_show_volume_overlay()` - Show HUD volume gauge overlay
- `formant_display_set_brightness()` - Adjust LCD backlight brightness
- `formant_display_clear()` - Clear display framebuffer
- `formant_display_get_sprite()` - Access underlying LGFX sprite
- `formant_display_set_mode()` - Switch display mode (TTS or CUSTOM)

### Widgets (Reusable)

All widgets in `include/widgets/` can be used independently:

- `curved_text_widget.h` - Text rendering on circular arcs
- `spectrum_widget.h` - Radial spectrum analyzer

## Examples

- `examples/full_tts_demo/` - Complete TTS visualization

## Architecture

```
formant_tts_display.h (Public API)
    ├── ui_controller.cpp (UI state machine & compositing)
    ├── display_hal.cpp (GC9A01 SPI driver & framebuffer)
    ├── theme.cpp (colors, radii, HSV conversion)
    ├── phoneme_renderer.cpp (center phoneme display)
    ├── ipa_tracker.cpp (top arc IPA string tracking)
    ├── word_tracker.cpp (bottom arc word tracking)
    ├── alphabet_ring.cpp (outer A-Z alphabet ring)
    ├── font_instances.cpp (native U8g2 fonts)
    └── widgets/ (generic circular widgets)
        ├── curved_text_widget.cpp
        └── spectrum_widget.cpp
```

## Dependencies

- ESP-IDF >= 5.0.0
- LovyanGFX (sprite rendering)
- esp_lcd_gc9a01 (display driver)

### Optional (for IPA font generation)

- u8g2 font tools (otf2bdf, bdfconv)
- Noto Sans font (Google Fonts, SIL OFL 1.1)

## Related Projects

- [esp32-p4-idf-formant-tts](https://github.com/Strg-Alt-Entf-0x00/esp32-p4-idf-formant-tts) - Formant synthesizer
- [esp-idf-espure-g2p](https://github.com/Strg-Alt-Entf-0x00/esp-idf-espure-g2p) - Grapheme-to-phoneme

## License

MIT License - see LICENSE file
