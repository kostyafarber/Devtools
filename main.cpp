#include "core/audio.h"

int main()
{
  core::AudioConfig config = {
      .sampling_rate = 44100, // Use standard audio rate
      .frequency = 440,
      .buffer_size = 1024,
  };

  core::AudioProcess audio("Audio", config);
  auto result = audio.initialise();
  if (result.is_error()) {
    return 1;
  }

  // Keep the process running
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
  ;
}