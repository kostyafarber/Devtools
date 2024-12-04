
#include "ipc/unix_socket.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>
class UnixSocketTest : public testing::Test
{
protected:
  void SetUp() override { m_socket_path = "/tmp/test_socket"; }

  std::string m_socket_path;
  struct Msg {
    static constexpr size_t size = 10;
    std::array<char, size> data;

    Msg() { std::copy_n("hello world", size, data.begin()); }
  };

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

  Msg msg;
  auto sent = client.send(msg.data.begin(), msg.size);

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
  auto server = std::move(maybe_accepted.value());

  Msg msg;
  auto sent = client.send(msg.data.begin(), msg.size);

  ASSERT_TRUE(sent);

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  Msg recv_msg;
  ;
  server.recv(recv_msg.data.begin(), recv_msg.size);

  ASSERT_TRUE(sent);

  std::array<char, msg.size> expected;
  std::copy_n("hello world", msg.size, expected.begin());

  ASSERT_EQ(recv_msg.data, expected);
}