#include "error.h"
#include "logging.h"
#include "message_frame.h"
#include "messages.pb.h"
#include "transport.h"
#include "unix_socket.h"
#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>
#include <tuple>

class TransportTest : public testing::Test
{
protected:
  void SetUp() override { m_socket_path = "/tmp/test_socket"; }

  std::string m_socket_path;

  base::ErrorOr<std::tuple<ipc::Transport, ipc::Transport>>
  connected_transporters()
  {
    auto maybe_socket = ipc::UnixSocket::create(m_socket_path);
    if (maybe_socket.is_error()) {
      LOG_AUDIO(Error, "error creating server socket: {}",
                maybe_socket.error().message());
      return maybe_socket.error();
    }

    auto socket = std::move(maybe_socket.value());
    auto maybe_listen = socket.listen();
    if (maybe_listen.is_error()) {
      LOG_AUDIO(Error, "error listening on server socket: {}",
                maybe_listen.error().message());
      return maybe_listen.error();
    }

    auto maybe_client = ipc::UnixSocket::create(m_socket_path);
    if (maybe_client.is_error()) {
      LOG_AUDIO(Error, "error creating client socket: {}",
                maybe_client.error().message());
      return maybe_client.error();
    }

    auto client = std::move(maybe_client.value());

    auto maybe_connect = client.connect();
    if (maybe_connect.is_error()) {
      LOG_AUDIO(Error, "failed to connect sockets: {}",
                maybe_connect.error().message());
      return maybe_connect.error();
    }

    auto maybe_accept = socket.accept();
    if (maybe_accept.is_error()) {
      LOG_AUDIO(Error, "failed to accept socket: {}",
                maybe_accept.error().message());
      return maybe_accept.error();
    }

    auto server = std::move(maybe_accept.value());

    ipc::Transport t_server(std::move(server));
    ipc::Transport t_client(std::move(client));

    return std::tuple{std::move(t_server), std::move(t_client)};
  }
  void TearDown() override { std::remove(m_socket_path.c_str()); }
};

TEST_F(TransportTest, SendCommand)
{
  auto maybe_sockets = connected_transporters();
  ASSERT_FALSE(maybe_sockets.is_error());

  auto [server, client] = std::move(maybe_sockets.value());

  synth::SynthMessage msg;
  msg.set_command(synth::SynthMessage::DECREASE_VOLUME);
  msg.set_volume(0.5);

  auto sent = client.try_send(msg);

  ASSERT_TRUE(sent);
}

TEST_F(TransportTest, RecvCommand)
{
  auto maybe_sockets = connected_transporters();
  ASSERT_FALSE(maybe_sockets.is_error());

  auto [server, client] = std::move(maybe_sockets.value());

  synth::SynthMessage msg;
  msg.set_command(synth::SynthMessage::DECREASE_VOLUME);
  msg.set_volume(0.5);

  auto sent = client.try_send(msg);
  ASSERT_TRUE(sent);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  synth::SynthMessage recv_msg;
  auto recv = server.try_recv(recv_msg);
  ASSERT_TRUE(recv);

  ASSERT_EQ(recv_msg.command(), synth::SynthMessage::DECREASE_VOLUME);
  ASSERT_EQ(recv_msg.volume(), 0.5);
}
