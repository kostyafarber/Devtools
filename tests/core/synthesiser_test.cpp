
#include "core/synthesiser.h"
#include <gtest/gtest.h>

TEST(Synthesiser, CreateSynthesiser)
{
    core::Synthesiser s(44100, 440 );
}