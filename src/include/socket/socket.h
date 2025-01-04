#ifndef ECOMM_SOCKET_H
#define ECOMM_SOCKET_H

#include <netinet/in.h>

#include <string>

namespace ecomm {

struct EndPoint {
  std::string ip;
  int port;

  EndPoint(const std::string &ip, const int port) : ip(ip), port(port) {}

  bool ToSockAddr(sockaddr_in *sock, socklen_t *sock_len) const;
};

class Socket {
public:
  Socket() : fd_(-1) {}
  ~Socket() {}

  bool Init();
  void Close();

  bool Bind(const EndPoint &server_end_point) const;
  bool Listen() const;

  bool Connect(const EndPoint &server_end_point, int connect_timeout_ms) const;

  int GetFD() const;

private:
  int fd_;

  bool isBlocking() const;
  void setNonBlocking() const;
  void setBlocking() const;

};  // class Socket

}  // namespace ecomm

#endif  // ECOMM_SOCKET_H
