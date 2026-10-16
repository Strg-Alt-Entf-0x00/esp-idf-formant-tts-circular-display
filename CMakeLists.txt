idf_component_register(
    SRCS
        "src/display_hal.cpp"
        "src/ui_controller.cpp"
        "src/theme.cpp"
        "src/utf8_utils.cpp"
        "src/phoneme_renderer.cpp"
        "src/ipa_tracker.cpp"
        "src/word_tracker.cpp"
        "src/alphabet_ring.cpp"
        "src/font_instances.cpp"
        "src/widgets/curved_text_widget.cpp"
        "src/widgets/spectrum_widget.cpp"
    INCLUDE_DIRS
        "include"
    PRIV_INCLUDE_DIRS
        "src"
    REQUIRES
        esp_lcd
        esp_timer
        driver
)

# Add C++17 support
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)

# Add Kconfig defaults
target_compile_definitions(${COMPONENT_LIB} PRIVATE
    CONFIG_FORMANT_DISPLAY_SPECTRUM_BARS=${CONFIG_FORMANT_DISPLAY_SPECTRUM_BARS}
)
