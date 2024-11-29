#include "core/audio.h"
#include "ipc/constants.h"
#include "ipc/messages/synth_message.h"
#include "ipc/socket.h"
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

  auto maybe_stop = a.stop();
  ASSERT_FALSE(maybe_stop.is_error());
}

TEST_F(AudioProccessTest, TestCommand)
{
  core::AudioProcess a("square", config);
  a.initialise();

  auto maybe_client = ipc::UnixSocket::create(ipc::constants::socket_path);
  ASSERT_FALSE(maybe_client.is_error());

  auto client = std::move(maybe_client.value());
  client.connect();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ipc::SynthMessage msg = {.m_message = ipc::SynthCommand::DecreaseVolume,
                           msg.data.volume = 0.25f};
  auto maybe_read = client.try_send(msg);
  ASSERT_TRUE(maybe_read);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  ASSERT_EQ(a.m_synth->volume(), 0.75f);
}

TEST_F(AudioProccessTest, TestBigCommandLoad)
{
  core::AudioProcess a("square", config);
  a.initialise();

  auto maybe_client = ipc::UnixSocket::create(ipc::constants::socket_path);
  ASSERT_FALSE(maybe_client.is_error());

  auto client = std::move(maybe_client.value());
  client.connect();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  for (int i = 0; i < 100; i++) {
    ipc::SynthMessage msg = {
        .m_message = ipc::SynthCommand::Stop,
    };
    auto maybe_read = client.try_send(msg);
    ASSERT_TRUE(maybe_read);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}