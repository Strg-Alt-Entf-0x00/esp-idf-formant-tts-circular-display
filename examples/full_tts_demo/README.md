# Full TTS Visualization Demo for ESP32-P4

Complete demonstration of real-time speech synthesis (TTS) visualization on a circular GC9A01 display (240x240).

## Features

- **Center Phoneme Display**: Real-time phonetic symbol rendering with IPA support and amplitude-reactive brightness
- **IPA Tracker** (top arc): Full phonetic transcription with character-level tracking and curved layout
- **Word Tracker** (bottom arc): Natural orthography with character-level synchronization
- **Alphabet Ring**: Outer A-Z circular ring highlighting active letters with halo glow
- **Formant Spectrum Analyzer**: 32 stationary radial bars with acoustic formant resonance and HSV rainbow colors
- **HUD Volume Gauge**: Circular overlay showing volume changes dynamically
- **Idle / Silence Transitions**: Automatic transition to customizable idle text during speech pauses

## Hardware Requirements

- ESP32-P4 development board (e.g., ESP32-P4-Function-EV-Board)
- GC9A01 240x240 round display (SPI interface)
- Connection wires
- Optional: Speaker / I2S DAC amplifier (when integrating full audio synthesis)

## Wiring & Pinout

Pins are fully configurable via `idf.py menuconfig` → **Example Display Configuration**.

### Default Pinout

| Display Pin (GC9A01) | ESP32-P4 (Default) | ESP32-S3 (Default) | Function |
|----------------------|--------------------|--------------------|----------|
| **MOSI** / SDA       | GPIO 28            | GPIO 11            | SPI Data Out |
| **SCLK** / SCL       | GPIO 27            | GPIO 12            | SPI Clock |
| **CS**               | GPIO 24            | GPIO 10            | Chip Select |
| **DC**               | GPIO 25            | GPIO 9             | Data / Command |
| **RST**              | GPIO 26            | GPIO 8             | Hardware Reset |
| **BL**               | -1 (disabled)      | -1 (disabled)      | Backlight PWM (-1 if tied to 3.3V) |
| **VCC**              | 3.3V               | 3.3V               | Power (3.3V) |
| **GND**              | GND                | GND                | Ground |

## What This Demo Does

### Simulated TTS Data

This demo cycles through example words with their authentic IPA transcriptions and synchronized phonemes:

- **Hallo** → IPA: `haˈloː`
- **Guten** → IPA: `ˈɡʊtən`
- **Tag** → IPA: `taːk`
- **Testing** → IPA: `ˈtɛstɪŋ`
- **World** → IPA: `wɜːld`

### Visual Layout

```
    ┌─────────────────────┐
    │ A B C D ... Y Z     │ ← Alphabet ring (highlights active letter)
    │                     │
    │      haˈloː         │ ← IPA tracker (top arc)
    │                     │
    │ ╱╲╱╲╱╲╱╲╱╲          │ ← Radial spectrum analyzer (32 bars)
    │                     │
    │        h            │ ← Center phoneme (large, prominent)
    │                     │
    │ ╲╱╲╱╲╱╲╱╲╱          │ ← Formant resonance
    │                     │
    │      Hallo          │ ← Word tracker (bottom arc)
    └─────────────────────┘
```

## Building and Flashing

```bash
# Set ESP-IDF target
idf.py set-target esp32p4

# Build project
idf.py build

# Flash and monitor
idf.py flash monitor
```

## Configuration

### Display Settings

Edit `sdkconfig.defaults` or run:

```bash
idf.py menuconfig
```

Under `Component config` → `Formant TTS Circular Display`:
- **TTS Visualization**: Enabled
- **Alphabet Ring**: Enabled
- **IPA Fonts**: Enabled (Noto Sans pre-rendered fonts)
- **Spectrum Bars**: 32 (range: 16–64)
- **PSRAM for Sprite Buffer**: Enabled (recommended for ESP32-P4)

### IPA Font Support

This example uses pre-rendered Noto Sans fonts for full phonetic Unicode support (ə, ʃ, θ, ˈ, etc.).
Pre-generated headers are included under `include/fonts/`. To regenerate or customize:

```bash
# On Linux / macOS / WSL:
tools/setup_and_generate_fonts.sh

# On Windows:
tools/generate_fonts_windows.bat
```

See [include/fonts/README.md](../../include/fonts/README.md) for details.

## Integration with Real TTS

To connect this display to a real TTS pipeline:

### 1. Add Dependencies

In `main/idf_component.yml`:

```yaml
dependencies:
  Strg-Alt-Entf-0x00/esp32-p4-idf-formant-tts: "^1.0.0"
  Strg-Alt-Entf-0x00/esp-idf-espure-g2p: "^1.0.0"
```

### 2. Stream Data to Display

Connect the TTS synthesizer output callback to `formant_display_set_tts_data()`:

```cpp
#include "formant_tts_display.h"

void on_phoneme_rendered(const char* word, const char* phoneme, const char* ipa, int word_idx, int ipa_idx, float amplitude) {
    formant_display_set_tts_data(amplitude, word, phoneme, ipa, word_idx, ipa_idx);
}
```

## Expected Monitor Output

```
I (123) tts_demo: ========================================
I (124) tts_demo: Formant TTS Circular Display Demo
I (125) tts_demo: ========================================
I (145) tts_demo: ✓ Display initialized
I (146) tts_demo: Showing initial volume HUD overlay (80%)...
I (2150) tts_demo: Starting continuous TTS visualization loop...
I (2800) tts_demo: Word: 'Hallo' | Phoneme: 'h' | IPA char: 0 | Word char: 0
I (3500) tts_demo: Word: 'Hallo' | Phoneme: 'a' | IPA char: 1 | Word char: 1
I (4200) tts_demo: Word: 'Hallo' | Phoneme: 'l' | IPA char: 3 | Word char: 2
I (4900) tts_demo: Word: 'Hallo' | Phoneme: 'oː' | IPA char: 4 | Word char: 4
I (5100) tts_demo: Word complete. Next word: 'Guten' (IPA: 'ˈɡʊtən')
```

## Performance & Memory

- **Target Resolution**: 240x240 circular (GC9A01)
- **Target Frame Rate**: 30 FPS stable
- **PSRAM Usage**: ~115 KB (16-bit double/sprite buffer)
- **Flash Usage**: ~75 KB for pre-rendered native IPA fonts (10pt, 12pt, 16pt)

## Related Projects

- [esp-idf-formant-clock-circular-display](https://github.com/Strg-Alt-Entf-0x00/esp-idf-formant-clock-circular-display) - Standalone analog clock display component
- [esp32-p4-idf-formant-tts](https://github.com/Strg-Alt-Entf-0x00/esp32-p4-idf-formant-tts) - Real-time formant speech synthesizer
- [esp-idf-espure-g2p](https://github.com/Strg-Alt-Entf-0x00/esp-idf-espure-g2p) - Grapheme-to-phoneme text conversion

## License

MIT License - See [LICENSE](../../LICENSE) in repository root.
