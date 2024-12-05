#include "core/audio.h"
#include "error.h"
#include "ipc/constants.h"
#include "ipc/transport.h"
#include "messages.pb.h"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

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

  base::ErrorOr<ipc::Transport> connected_transporter()
  {
    auto maybe_client = ipc::UnixSocket::create(ipc::constants::socket_path);
    if (maybe_client.is_error()) {
      return maybe_client.error();
    }

    auto client = std::move(maybe_client.value());
    auto maybe_connect = client.connect();
    if (maybe_connect.is_error())
      return maybe_client.error();

    return ipc::Transport(std::move(client));
  }
};

TEST_F(AudioIntegrationTest, AudioCommand)
{
  core::AudioProcess a("square", config);
  a.initialise();

  auto maybe_transporter = connected_transporter();
  ASSERT_FALSE(maybe_transporter.is_error());

  auto transporter = std::move(maybe_transporter.value());

  synth::SynthMessage msg;
  msg.set_command(synth::SynthMessage::DECREASE_VOLUME);
  msg.set_volume(0.5f);

  auto sent = transporter.try_send(msg);
  ASSERT_TRUE(sent);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ASSERT_EQ(0.5f, a.m_synth->volume());
}