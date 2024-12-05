#include "base/ring_buffer.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

TEST(RingBuffer, WriteItem)
{
  base::RingBuffer<float> buffer(3);

  auto in_buffer = std::vector<float>{1};
  auto ok = buffer.write(&in_buffer[0]);

  ASSERT_TRUE(ok);
}

TEST(RingBuffer, WriteNItems)
{
  base::RingBuffer<float> buffer(3);

  auto in_buffer = std::vector<float>{1, 2};
  auto ok = buffer.write(&in_buffer[0], in_buffer.size());

  ASSERT_TRUE(ok);
}

TEST(RingBuffer, WriteGreaterThanAvailableSpace)
{
  base::RingBuffer<float> buffer(3);

  auto in_buffer = std::vector<float>{1, 2, 3, 4};
  auto ok = buffer.write(&in_buffer[0], in_buffer.size());

  ASSERT_TRUE(ok);
}

TEST(RingBuffer, ReadItem)
{
  base::RingBuffer<float> buffer(3);
  auto in_buffer = std::vector<float>{1, 2};
  buffer.write(&in_buffer[0], in_buffer.size());

  auto out_buffer = std::vector<float>(1);
  auto result = buffer.read(&out_buffer[0]);

  ASSERT_TRUE(result);
  ASSERT_EQ(out_buffer[0], 1);
}

TEST(RingBuffer, ReadNItems)
{
  base::RingBuffer<float> buffer(3);
  auto in_buffer = std::vector<float>{1, 2, 3};
  buffer.write(&in_buffer[0], in_buffer.size());

  auto out_buffer = std::vector<float>(2);
  auto result = buffer.read(&out_buffer[0], out_buffer.size());

  ASSERT_TRUE(result);
  ASSERT_THAT(out_buffer, testing::ElementsAre(1, 2));
}

TEST(RingBuffer, WriteWrapAround)
{
  base::RingBuffer<float> buffer(3);

  auto in_buffer = std::vector<float>{1, 2, 3, 4};
  auto ok = buffer.write(&in_buffer[0], in_buffer.size());

  ASSERT_TRUE(ok);

  auto out_buffer = std::vector<float>(2);
  ok = buffer.read(&out_buffer[0], out_buffer.size());

  ASSERT_TRUE(ok);

  ASSERT_THAT(out_buffer, testing::ElementsAre(4, 2));
}

TEST(RingBuffer, ReadWhenEmptyShouldError)
{
  base::RingBuffer<float> buffer(3);
  auto ok = buffer.read(nullptr);

  ASSERT_FALSE(ok);
}