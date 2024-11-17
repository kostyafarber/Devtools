#include "ru/ring_buffer.h"
#include <gtest/gtest.h>

TEST(RingBuffer, WrapAround) { ru::RingBuffer buffer(3); }
