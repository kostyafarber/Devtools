
#include "core/synthesiser.h"
#include <gtest/gtest.h>

TEST(Synthesiser, CreateSynthesiser) { core::Synthesiser s(44100, 440, 0.5f); }

TEST(Synthesiser, PhaseLessThanDutyCycle)
{
  core::Synthesiser s(44100, 440, 0.5f);

  for (int i = 0; i < 40; i++) {
    s.generate();
  }

  ASSERT_EQ(s.generate(), -1.0f);
}

TEST(Synthesiser, PhaseGreaterThanDutyCycle)
{
  core::Synthesiser s(44100, 440, 0.5f);

  for (int i = 0; i < 51; i++) {
    s.generate();
  }

  ASSERT_EQ(s.generate(), 1.0f);
}
