#include "core/audio.h"
#include <gtest/gtest.h>
#include <mach/clock_types.h>
#include <unistd.h>

TEST(AudioProcess, Initalise)
{
  core::AudioProcess a("audio");

  auto result = a.initialise().is_error();
  ASSERT_FALSE(result);

  sleep(2);
  }