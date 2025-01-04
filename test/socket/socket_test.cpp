#include "socket/socket.h"

#include <thread>

#include <gtest/gtest.h>
#include <util/scope_defer.h>


TEST (Socket, listen_connect) {
  const auto server_addr = ecomm::EndPoint{"127.0.0.1", 10000};
  ecomm::Socket server_socket;
  ASSERT_EQ(server_socket.Init(), true);
  scope_defer {
    server_socket.Close();
  };
  ASSERT_EQ(server_socket.Bind(server_addr), true);
  ASSERT_EQ(server_socket.Listen(), true);
  std::thread server_thread([&]() {
    const int listen_fd = server_socket.GetFD();
    sockaddr_in addr;
    socklen_t addr_size = sizeof(addr);
    return accept(listen_fd, reinterpret_cast<sockaddr *>(&addr), &addr_size);
  });

  ecomm::Socket client_socket;
  ASSERT_EQ(client_socket.Init(), true);
  scope_defer {
    client_socket.Close();
  };
  ASSERT_EQ(client_socket.Connect(server_addr, 1000), true);
  server_thread.join();
}

TEST (Socket, connect_refused) {
  const auto server_addr = ecomm::EndPoint{"127.0.0.1", 10001};
  ecomm::Socket client_socket;
  ASSERT_EQ(client_socket.Init(), true);
  scope_defer {
    client_socket.Close();
  };
  ASSERT_EQ(client_socket.Connect(server_addr, 10000), false);
  ASSERT_EQ(errno, ECONNREFUSED);
}

TEST (Socket, connect_timeout) {
  const auto server_addr = ecomm::EndPoint{"8.8.8.8", 10001};
  ecomm::Socket client_socket;
  ASSERT_EQ(client_socket.Init(), true);
  scope_defer {
    client_socket.Close();
  };
  ASSERT_EQ(client_socket.Connect(server_addr, 1), false);
  ASSERT_EQ(errno, ETIMEDOUT);
}