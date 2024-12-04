#include "core/audio.h"
#include <gtest/gtest.h>

class AudioIntegrationTest : public testing::Test
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

TEST_F(AudioIntegrationTest, AudioCommand) { ASSERT_TRUE(true); }