#include "core/audio.h"
#include <chrono>
#include <gtest/gtest.h>
#include <mach/clock_types.h>
#include <thread>
#include <unistd.h>

class AudioProccessTest : public testing::Test
{
protected:
  core::AudioConfig config;

  void SetUp() override
  {
    config = {
        .frequency = 440,
        .sampling_rate = 44100,
        .buffer_size = 1024,
    };
  }
};

TEST_F(AudioProccessTest, Initalise)
{

  core::AudioProcess a("square", config);

  auto result = a.initialise().is_error();
  ASSERT_FALSE(result);
}

TEST_F(AudioProccessTest, Play)
{
  core::AudioProcess a("square", config);
  auto result = a.initialise().is_error();
  ASSERT_FALSE(result);

  auto maybe_play = a.play();
  ASSERT_FALSE(maybe_play.is_error());
}

TEST_F(AudioProccessTest, Stop)
{
  core::AudioProcess a("square", config);
  a.initialise();

  auto maybe_play = a.play();
  ASSERT_FALSE(maybe_play.is_error());

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  auto maybe_stop = a.stop();
  ASSERT_FALSE(maybe_stop.is_error());
}