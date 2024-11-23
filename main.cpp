#include "core/audio.h"
#include <chrono>
#include <iostream>
#include <thread>

int main()
{
  core::AudioConfig config = {
      .sampling_rate = 14400,
      .frequency = 440,
      .buffer_size = 1024,
  };

  core::AudioProcess audio("Audio", config);

  if (auto maybe_audio = audio.initialise(); maybe_audio.is_error())
    std::cout << "error initialising" << std::endl;

  if (auto maybe_play = audio.play(); maybe_play.is_error())
    std::cout << "error playing" << std::endl;

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}