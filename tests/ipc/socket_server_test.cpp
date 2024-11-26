
#include "ipc/socket_server.h"
#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>

class SocketServerTest : public testing::Test
{
protected:
  void SetUp() override { m_socket_path = "/tmp/test_socket"; }

  std::string m_socket_path;

  void TearDown() override { std::remove(m_socket_path.c_str()); }
};

TEST_F(SocketServerTest, Create)
{
  auto maybe_server = ipc::SocketServer::create(m_socket_path);
  ASSERT_FALSE(maybe_server.is_error());
}

TEST_F(SocketServerTest, ConnectClient)
{
  auto maybe_server = ipc::SocketServer::create(m_socket_path);
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
  auto maybe_server = ipc::SocketServer::create(m_socket_path);
  ASSERT_FALSE(maybe_server.is_error());

  auto server = std::move(maybe_server.value());
  auto maybe_client = ipc::UnixSocket::create(m_socket_path);
  ASSERT_FALSE(maybe_client.is_error());

  auto client = std::move(maybe_client.value());
  auto maybe_connect = client.connect();
  ASSERT_FALSE(maybe_connect.is_error());

  ipc::SynthMessage msg;
  msg.m_message = ipc::SynthCommand::IncreaseVolume;
  msg.data.volume = 0.2;

  auto maybe_send = client.try_send(msg);
  ASSERT_TRUE(maybe_send);

  ipc::SynthMessage recv_msg;
}