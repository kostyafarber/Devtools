
#include "ipc/messages/message_header.h"
#include "ipc/unix_socket.h"
#include "messages.pb.h"
#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>
class UnixSocketTest : public testing::Test
{
protected:
  void SetUp() override { m_socket_path = "/tmp/test_socket"; }

  std::string m_socket_path;

  void TearDown() override { std::remove(m_socket_path.c_str()); }
};

TEST_F(UnixSocketTest, Create)
{
  auto socket = ipc::UnixSocket::create(m_socket_path);
  ASSERT_FALSE(socket.is_error());
}

TEST_F(UnixSocketTest, Listen)
{
  auto maybe_socket = ipc::UnixSocket::create(m_socket_path);
  ASSERT_FALSE(maybe_socket.is_error());

  auto socket = std::move(maybe_socket.value());
  auto maybe_listen = socket.listen();

  ASSERT_FALSE(maybe_listen.is_error());
}

TEST_F(UnixSocketTest, Connect)
{
  auto maybe_socket = ipc::UnixSocket::create(m_socket_path);
  ASSERT_FALSE(maybe_socket.is_error());

  auto socket = std::move(maybe_socket.value());
  auto maybe_listen = socket.listen();
  ASSERT_FALSE(maybe_listen.is_error());

  auto maybe_client = ipc::UnixSocket::create(m_socket_path);
  ASSERT_FALSE(maybe_client.is_error());
  auto client = std::move(maybe_client.value());

  auto maybe_connect = client.connect();
  ASSERT_FALSE(maybe_connect.is_error());
}

TEST_F(UnixSocketTest, Send)
{
  auto maybe_socket = ipc::UnixSocket::create(m_socket_path);
  ASSERT_FALSE(maybe_socket.is_error());

  auto socket = std::move(maybe_socket.value());
  auto maybe_listen = socket.listen();
  ASSERT_FALSE(maybe_listen.is_error());

  auto maybe_client = ipc::UnixSocket::create(m_socket_path);
  ASSERT_FALSE(maybe_client.is_error());
  auto client = std::move(maybe_client.value());

  auto maybe_connect = client.connect();
  ASSERT_FALSE(maybe_connect.is_error());

  ipc::MessageHeader::Raw header;
  synth::SynthMessage msg;

  msg.set_command(synth::SynthMessage::INCREASE_VOLUME);
  msg.set_volume(0.5f);

  auto sent = client.try_send(header, msg);

  ASSERT_TRUE(sent);
}

TEST_F(UnixSocketTest, Receive)
{
  auto maybe_socket = ipc::UnixSocket::create(m_socket_path);
  ASSERT_FALSE(maybe_socket.is_error());

  auto socket = std::move(maybe_socket.value());
  auto maybe_listen = socket.listen();
  ASSERT_FALSE(maybe_listen.is_error());

  auto maybe_client = ipc::UnixSocket::create(m_socket_path);
  ASSERT_FALSE(maybe_client.is_error());
  auto client = std::move(maybe_client.value());

  auto maybe_connect = client.connect();
  ASSERT_FALSE(maybe_connect.is_error());

  auto maybe_accepted = socket.accept();
  ASSERT_FALSE(maybe_accepted.is_error());
  auto accepted_socket = std::move(maybe_accepted.value());

  ipc::MessageHeader::Raw header{.magic = ipc::MessageHeader::MAGIC, .size = 0};
  synth::SynthMessage msg;

  msg.set_command(synth::SynthMessage::INCREASE_VOLUME);
  msg.set_volume(0.5f);

  auto sent = client.try_send(header, msg);

  ASSERT_TRUE(sent);

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  ipc::MessageHeader::Raw recv_header;
  synth::SynthMessage recv_msg;
  sent = accepted_socket.try_recv(recv_header, recv_msg);

  ASSERT_TRUE(sent);
  

  ASSERT_EQ(recv_msg.command(), val2)
}