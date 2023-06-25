#ifndef ASYNCGI_SERVER_H
#define ASYNCGI_SERVER_H

#include "requestprocessor.h"
#include <filesystem>
#include <memory>
#include <string_view>

namespace asyncgi {
class IO;

namespace detail {
class ServerService;
}

class Server {
public:
    Server(IO&, RequestProcessor);
    ~Server();
    Server(const Server&) = delete;
    Server(Server&&) = default;
    Server& operator=(const Server&) = delete;
    Server& operator=(Server&&) = default;

    void listen(std::string_view ipAddress, int port);
    void listen(const std::filesystem::path& unixDomainSocket);

private:
    std::unique_ptr<detail::ServerService> serverService_;
};

} //namespace asyncgi

#endif //ASYNCGI_SERVER_H
