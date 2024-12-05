# Mac bundle configuration
# Add this function to handle icon conversion and copying


function(configure_mac_bundle TARGET_NAME UI_BINARY BACKEND_BINARY)
    set(BUNDLE_NAME "SynthUI")
    set(BUNDLE_PATH "${CMAKE_BINARY_DIR}/${BUNDLE_NAME}.app")

    # Create bundle structure
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${BUNDLE_PATH}/Contents/MacOS"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${BUNDLE_PATH}/Contents/Resources"
    )

    # Copy binaries
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${BACKEND_BINARY}"
        "${BUNDLE_PATH}/Contents/MacOS/SynthBackend"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${UI_BINARY}"
        "${BUNDLE_PATH}/Contents/MacOS/SynthUI"
    )

    # Generate Info.plist
    configure_file(
        "${CMAKE_SOURCE_DIR}/Info.plist.in"
        "${BUNDLE_PATH}/Contents/Info.plist"
        @ONLY
    )

    # Create launcher script
    file(WRITE "${BUNDLE_PATH}/Contents/MacOS/launcher.sh"
        "#!/bin/bash\n"
        "DIR=\"$(cd \"$(dirname \"\${BASH_SOURCE[0]}\")\" && pwd)\"\n"
        "\"$DIR/SynthBackend\" &\n"
        "sleep 1\n"
        "\"$DIR/SynthUI\"\n"
    )

    # Make launcher executable
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND chmod +x "${BUNDLE_PATH}/Contents/MacOS/launcher.sh"
    )
endfunction()