#!/bin/bash
# Complete automated font generation setup for ESP32 IPA display
# This script downloads everything needed and generates the fonts

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "=========================================="
echo "ESP32 IPA Font Generator - Full Setup"
echo "=========================================="
echo ""

# Create temp directory
TEMP_DIR="./font_build_temp"
mkdir -p "$TEMP_DIR"
cd "$TEMP_DIR"

echo "[1/6] Checking build dependencies..."
# Check for required packages
NEEDS_UPDATE=0

if ! command -v gcc &> /dev/null; then
    echo "Need to install build-essential..."
    NEEDS_UPDATE=1
fi

if ! command -v wget &> /dev/null; then
    echo "Need to install wget..."
    NEEDS_UPDATE=1
fi

if ! dpkg -l | grep -q libfreetype6-dev; then
    echo "Need to install FreeType2 development libraries..."
    NEEDS_UPDATE=1
fi

if [ $NEEDS_UPDATE -eq 1 ]; then
    echo "Installing required packages..."
    sudo apt-get update
    sudo apt-get install -y build-essential wget libfreetype6-dev git
fi

echo "✓ Build tools available"
echo ""

echo "[2/6] Downloading u8g2 font tools..."
if [ ! -d "u8g2" ]; then
    git clone https://github.com/olikraus/u8g2.git
else
    echo "✓ u8g2 already downloaded"
fi
echo ""

echo "[3/6] Building otf2bdf..."
cd u8g2/tools/font/otf2bdf
if [ ! -f "otf2bdf" ]; then
    make
    echo "✓ otf2bdf built"
else
    echo "✓ otf2bdf already built"
fi
OTF2BDF_PATH="$(pwd)/otf2bdf"
cd "$SCRIPT_DIR/$TEMP_DIR"
echo ""

echo "[4/6] Building bdfconv..."
cd u8g2/tools/font/bdfconv
if [ ! -f "bdfconv" ]; then
    make
    echo "✓ bdfconv built"
else
    echo "✓ bdfconv already built"
fi
BDFCONV_PATH="$(pwd)/bdfconv"
cd "$SCRIPT_DIR/$TEMP_DIR"
echo ""

echo "[5/6] Downloading Noto Sans font..."
FONT_FILE="NotoSans-Regular.ttf"
if [ ! -f "$FONT_FILE" ]; then
    echo "Downloading from Google Fonts..."
    
    # Try static font first (best for embedded systems)
    if ! wget -O "$FONT_FILE" "https://github.com/google/fonts/raw/main/ofl/notosans/static/NotoSans-Regular.ttf" 2>/dev/null; then
        echo "Static font not available, downloading variable font..."
        wget -O "NotoSans.ttf" "https://github.com/google/fonts/raw/main/ofl/notosans/NotoSans%5Bwdth%2Cwght%5D.ttf"
        mv "NotoSans.ttf" "$FONT_FILE"
    fi
    
    echo "✓ Font downloaded"
else
    echo "✓ Font already downloaded"
fi

if [ ! -f "$FONT_FILE" ]; then
    echo "ERROR: Could not download font!"
    echo "Please manually download NotoSans-Regular.ttf from:"
    echo "https://fonts.google.com/noto/specimen/Noto+Sans"
    echo "and place it in: $SCRIPT_DIR/$TEMP_DIR/"
    exit 1
fi
echo ""

echo "[6/6] Generating IPA fonts..."
OUTPUT_DIR="$SCRIPT_DIR/../include/fonts"
mkdir -p "$OUTPUT_DIR"

# Character ranges for IPA-capable fonts (12pt, 16pt)
# 0x20-0x7F:   Basic Latin (ASCII)
# 0xA0-0x17F:  Latin-1 Supplement + Latin Extended-A (ä, ö, ü, etc.)
# 0x250-0x2AF: IPA Extensions (ə, ɪ, ʃ, θ, ɛ, ɜ, ʊ, ŋ, etc.)
# 0x2B0-0x2FF: Spacing Modifier Letters (ˈ primary stress, ˌ secondary stress, ː length mark)
# 0x300-0x36F: Combining Diacritical Marks
# 0x391-0x3DB: Greek (for phonetic notation)
IPA_RANGE="32-127,160-383,592-767,768-879,913-987"

# Character range for small fonts (8pt) - ASCII + Latin-1 only
SMALL_RANGE="32-127,160-255"

# Generate IPA fonts at 12pt and 16pt
for SIZE in 12 16; do
    echo ""
    echo "Generating ${SIZE}pt IPA font..."
    
    # Step 1: Convert TTF to BDF
    BDF_FILE="noto_sans_${SIZE}.bdf"
    echo "  Converting TTF → BDF..."
    "$OTF2BDF_PATH" -a -r 72 -p ${SIZE} -o "$BDF_FILE" "$FONT_FILE"
    
    # Step 2: Convert BDF to u8g2 C array
    C_FILE="$OUTPUT_DIR/noto_sans_ipa_${SIZE}.h"
    echo "  Converting BDF → C header..."
    "$BDFCONV_PATH" -v -f 1 -b 0 -m "$IPA_RANGE" "$BDF_FILE" -o "$C_FILE" -n "noto_sans_ipa_${SIZE}"
    
    # Cleanup
    rm "$BDF_FILE"
    
    echo "  ✓ Generated: $C_FILE"
done

# Generate 10pt font for alphabet ring scale numbers
echo ""
echo "Generating 10pt alphabet ring font..."
BDF_FILE="noto_sans_10.bdf"
"$OTF2BDF_PATH" -a -r 72 -p 10 -o "$BDF_FILE" "$FONT_FILE"
"$BDFCONV_PATH" -v -f 1 -b 0 -m "$IPA_RANGE" "$BDF_FILE" -o "$OUTPUT_DIR/noto_sans_ipa_10.h" -n "noto_sans_ipa_10"
rm "$BDF_FILE"
echo "  ✓ Generated: $OUTPUT_DIR/noto_sans_ipa_10.h"



echo ""
echo "=========================================="
echo "✓ Font Generation Complete!"
echo "=========================================="
echo ""
echo "Generated files:"
echo "  - include/fonts/noto_sans_ipa_10.h (alphabet ring, ~6.8KB)"
echo "  - include/fonts/noto_sans_ipa_12.h (arc text, ~8.6KB)"
echo "  - include/fonts/noto_sans_ipa_16.h (center phoneme, ~10.8KB)"
echo ""
echo "Font details:"
echo "  - Character coverage: ASCII + Latin Extended + IPA Extensions"
echo "  - Includes: ə ɪ ʃ θ ɛ ɜ ʊ ŋ ˈ ˌ ː (all IPA symbols)"
echo "  - Size: ~26KB total (stored in Flash)"
echo "  - Format: u8g2 C arrays (native pixel rendering, no scaling)"
echo ""
echo "Next steps:"
echo "1. Enable IPA fonts in menuconfig:"
echo "   CONFIG_FORMANT_DISPLAY_IPA_FONT=y"
echo ""
echo "2. Build your ESP-IDF project:"
echo "   idf.py build"
echo ""
echo "Cleanup:"
echo "You can safely delete: $SCRIPT_DIR/$TEMP_DIR/"
echo "(Keeping it allows faster re-generation)"
echo ""
