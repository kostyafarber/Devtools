
#include "base/ring_buffer.h"
#include "ipc/socket_server.h"
#include "messages.pb.h"
#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>

class SocketServerTest : public testing::Test
{
protected:
  void SetUp() override { m_socket_path = "/tmp/test_socket"; }

  std::string m_socket_path;

  base::RingBuffer<ipc::SynthMessage> m_command_queue{5};

  void TearDown() override { std::remove(m_socket_path.c_str()); }
};

TEST_F(SocketServerTest, Create)
{
  auto maybe_server = ipc::SocketServer::create(m_socket_path, m_command_queue);
  ASSERT_FALSE(maybe_server.is_error());
}

TEST_F(SocketServerTest, ConnectClient)
{
  auto maybe_server = ipc::SocketServer::create(m_socket_path, m_command_queue);
  ASSERT_FALSE(maybe_server.is_error());

  auto server = std::move(maybe_server.value());
  auto maybe_client = ipc::UnixSocket::create(m_socket_path);
  ASSERT_FALSE(maybe_client.is_error());

  auto client = std::move(maybe_client.value());
  auto maybe_connect = client.connect();
  ASSERT_FALSE(maybe_connect.is_error());
}

TEST_F(SocketServerTest, SendReceive)
{
  auto maybe_server = ipc::SocketServer::create(m_socket_path, m_command_queue);
  ASSERT_FALSE(maybe_server.is_error());

  auto server = std::move(maybe_server.value());
  auto maybe_client = ipc::UnixSocket::create(m_socket_path);
  ASSERT_FALSE(maybe_client.is_error());

  server.start();

  auto client = std::move(maybe_client.value());
  auto maybe_connect = client.connect();
  ASSERT_FALSE(maybe_connect.is_error());
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ipc::SynthMessage msg;

  synth::SynthMessage message;
  message.set_command(synth::SynthMessage::INCREASE_VOLUME);
  message.set_volume(0.5);

  msg.m_message = ipc::SynthCommand::IncreaseVolume;
  msg.data.volume = 0.2;

  auto maybe_send = client.try_send(msg);
  ASSERT_TRUE(maybe_send);

  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  ipc::SynthMessage recv_msg;
  auto recv = m_command_queue.read(&recv_msg);

  server.stop();

  ASSERT_TRUE(recv);
  ASSERT_EQ(recv_msg.data.volume, 0.2f);
  ASSERT_EQ(recv_msg.m_message, ipc::SynthCommand::IncreaseVolume);
}