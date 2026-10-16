# Fonts Directory

## Current Fonts (Built-in LovyanGFX)

- `FreeSans9pt7b` - ASCII only (default fallback)

## IPA Fonts (To be Generated)

These fonts are pre-generated or can be regenerated using `tools/setup_and_generate_fonts.sh` (Linux/macOS) or `tools/generate_fonts_windows.bat` (Windows):

### Automated Generation

The script automatically downloads Noto Sans and compiles the u8g2 font tools:

```bash
# On Linux / macOS / WSL:
cd tools
./setup_and_generate_fonts.sh

# On Windows:
cd tools
generate_fonts_windows.bat
```

This generates native pixel-perfect headers:
- `noto_sans_ipa_10.h` - 10pt font for alphabet ring markers (~18KB)
- `noto_sans_ipa_12.h` - 12pt font for arc text (~24KB)
- `noto_sans_ipa_16.h` - 16pt font for center phoneme (~32KB)

## Font Character Coverage

### IPA Extensions (U+0250–U+02AF)
Vowels: ə ɪ ʊ ɛ ɔ æ ɑ ʌ ɒ ʏ œ
Consonants: ʃ ʒ θ ð ŋ ʔ ɹ ɾ

### Combining Diacriticals (U+0300–U+036F)
Stress: ˈ ˌ
Length: ː
Tones: ́ ̀ ̂ ̃

### Latin Extended-A (U+0100–U+017F)
German: ä ö ü ß
French: à é è ê ç
And more...

## Usage in Code

```cpp
// Include generated font
#include "fonts/noto_sans_ipa_12.h"

// Use with LovyanGFX sprite
sprite->setFont(noto_sans_ipa_12);
sprite->drawString("həˈloʊ", x, y);  // Renders IPA correctly!
```

## License

**Noto Sans**: SIL Open Font License 1.1
- Free for commercial use
- Can be embedded and redistributed
- See `LICENSE_NOTO.txt`

**Generated u8g2 fonts**: Same license as source font (SIL OFL 1.1)
