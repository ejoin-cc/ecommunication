#include "socket/socket.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <cstring>

#include "util/scope_defer.h"
#include "util/time.h"

namespace ecomm {

bool EndPoint::ToSockAddr(sockaddr_in *sock, socklen_t *sock_len) const {
  bzero(sock, sizeof(*sock));
  sock->sin_family = AF_INET;
  if (inet_aton(this->ip.c_str(), &sock->sin_addr) == 0) {
    return false;
  }
  sock->sin_port = htons(this->port);
  if (sock_len) {
    *sock_len = sizeof(*sock);
  }
  return true;
}

bool Socket::Init() {
  fd_ = socket(PF_INET, SOCK_STREAM, 0);
  if (fd_ < 0) {
    return false;
  }
  return true;
}

void Socket::Close() {
  if (fd_ < 0) {
    return;
  }
  close(fd_);
}

bool Socket::Bind(const EndPoint &server_end_point) const {
  if (fd_ < 0) {
    return false;
  }
  sockaddr_in addr;
  socklen_t addr_size = 0;
  if (!server_end_point.ToSockAddr(&addr, &addr_size)) {
    return false;
  }
  if (bind(fd_, reinterpret_cast<sockaddr *>(&addr), addr_size) != 0) {
    return false;
  }
  return true;
}

bool Socket::Listen() const {
  if (fd_ < 0) {
    return false;
  }
  if (listen(fd_, 65535) != 0) {
    return false;
  }
  return true;
}

bool Socket::Connect(const EndPoint &server_end_point,
                     int connect_timeout_ms) const {
  if (fd_ < 0) {
    return false;
  }

  if (connect_timeout_ms == -1) {
    connect_timeout_ms = 1000;
  }
  const auto timeout_time_point =
      GetTimePointFromNow(std::chrono::milliseconds(connect_timeout_ms));

  const bool is_blocking = isBlocking();
  if (is_blocking) {
    setNonBlocking();
  }
  scope_defer {
    if (is_blocking) {
      setBlocking();
    }
  };

  sockaddr_in addr;
  socklen_t addr_size = 0;
  if (!server_end_point.ToSockAddr(&addr, &addr_size)) {
    return false;
  }

  int ret = connect(fd_, reinterpret_cast<sockaddr *>(&addr), addr_size);
  if (ret == 0) {
    return true;
  }
  if (errno != EINPROGRESS) {
    return false;
  }

  pollfd pfd = {fd_, POLLOUT, 0};
  while (true) {
    auto now_time_point = GetNowTimePoint();
    if (now_time_point >= timeout_time_point) {
      errno = ETIMEDOUT;
      return false;
    }
    const int remain_time_ms =
        GetTimeSpan(now_time_point, timeout_time_point).count();
    ret = poll(&pfd, 1, remain_time_ms);
    if (ret > 0) {
      break;
    }
    if (ret == 0) {
      errno = ETIMEDOUT;
      return false;
    }
    if (errno == EINTR) {
      continue;
    }
    return false;
  }
  if (pfd.revents & POLLNVAL) {
    errno = EBADF;
    return false;
  }
  errno = 0;
  int error = 0;
  socklen_t err_len = sizeof(error);
  if (getsockopt(fd_, SOL_SOCKET, SO_ERROR, &error, &err_len) < 0) {
    return false;
  }
  if (error != 0) {
    errno = error;
    return false;
  }
  return true;
}

bool Socket::isBlocking() const {
  const int flags = fcntl(fd_, F_GETFL, 0);
  return flags >= 0 && !(flags & O_NONBLOCK);
}

void Socket::setNonBlocking() const {
  const int flags = fcntl(fd_, F_GETFL, 0);
  if (flags < 0) {
    return;
  }
  if (flags & O_NONBLOCK) {
    return;
  }
  fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
}

void Socket::setBlocking() const {
  const int flags = fcntl(fd_, F_GETFL, 0);
  if (flags < 0) {
    return;
  }
  if (flags & O_NONBLOCK) {
    fcntl(fd_, F_SETFL, flags & ~O_NONBLOCK);
  }
}

int Socket::GetFD() const { return fd_; }

}  // namespace ecomm