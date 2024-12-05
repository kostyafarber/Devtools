
#include "base/ring_buffer.h"
#include "ipc/command_server.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

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
  auto maybe_server =
      ipc::CommandServer::create(m_socket_path, m_command_queue);
  ASSERT_FALSE(maybe_server.is_error());
}
