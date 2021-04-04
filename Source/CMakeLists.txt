

# juce_set_aax_sdk_path(~/SDKs/AAX)

juce_add_plugin(BITMANIP
        # VERSION ...                               # Set this if the plugin version is different to the project version
        # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
        # ICON_SMALL ...
        COMPANY_NAME zactowbes                          # Specify the name of the plugin's author
        IS_SYNTH FALSE                       # Is this a synth or an effect?
        NEEDS_MIDI_INPUT FALSE               # Does the plugin need midi input?
        NEEDS_MIDI_OUTPUT FALSE              # Does the plugin need midi output?
        IS_MIDI_EFFECT FALSE                 # Is this plugin a MIDI effect?
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE    # Does the editor need keyboard focus?
        COPY_PLUGIN_AFTER_BUILD FALSE        # Should the plugin be installed to a default location after building?
        PLUGIN_MANUFACTURER_CODE zAct               # A four-character manufacturer id with at least one upper-case character
        PLUGIN_CODE B1ty                           # A unique four-character plugin id with exactly one upper-case character
        FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
        PRODUCT_NAME "bitmanip")        # The name of the final executable, which can differ from the target name


juce_generate_juce_header(BITMANIP)



target_sources(BITMANIP
        PRIVATE
        PluginProcessor.cpp
        PluginEditor.cpp
        HysteresisProcessor.cpp
        BitmanipProcessor.cpp
        )

target_compile_definitions(BITMANIP
        PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
        )

target_link_libraries(BITMANIP
        PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        juce::juce_audio_utils
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        juce::juce_dsp
        )
