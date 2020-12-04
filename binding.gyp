{
    "targets": [{
        "target_name": "AudioManager",
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        "sources": [
            "cpp/main.cpp",
            "cpp/SoundRecorder.cpp"
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")"
        ],
        'libraries': [
            # -lpulse-simple is definitely needed for this program
            # I'm not sure sure about -lpulse so it's commented out for now
            # "/usr/lib/aarch64-linux-gnu/libpulse.so",
            "/usr/lib/aarch64-linux-gnu/libpulse-simple.so"
        ],
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")"
        ],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }]
}