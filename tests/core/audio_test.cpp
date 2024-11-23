#include "core/audio.h"
#include <gtest/gtest.h>
#include <mach/clock_types.h>
#include <unistd.h>

class AudioProccessTest :public testing::Test {
  protected:
    void SetUp() override {
      config = {
        .frequency = 440,
        .sampling_rate = 44100,
      };
    }
    core::AudioConfig config;
};

TEST(AudioProcessTest, Initalise)
{

  core::AudioProcess a("square", config);

  auto result = a.initialise().is_error();
  ASSERT_FALSE(result);
}

TEST(AudioProcessTest, Play) {}
TEST(AudioProcessTest, Stop) {}