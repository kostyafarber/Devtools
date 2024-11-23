#include "core/audio.h"

int main() {
    core::AudioConfig config = {
        .sampling_rate = 14400,
        .frequency = 440,
    };

    core::AudioProcess audio("Audio", config);

    audio.initialise();
    audio.play();

}