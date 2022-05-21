#pragma once
#include "alias_unixdomain.h"
#include <asyncgi/iclient.h>
#include <asyncgi/errors.h>
#include <hot_teacup/request.h>
#include <hot_teacup/response.h>
#include <fcgi_responder/requester.h>
#include <filesystem>

namespace asyncgi{
namespace fs = std::filesystem;

class Client : public IClient, public fcgi::Requester {
public:
    Client(asio::io_context&, ErrorHandlerFunc);
    void makeRequest(const fs::path& socketPath,
                     const FCGIRequest& fcgiRequest,
                     const std::function<void(const std::optional<FCGIResponse>&)>& responseHandler) override;
    void makeRequest(const fs::path& socketPath,
                     const http::Request& request,
                     const std::function<void(const std::optional<http::Response>&)>& responseHandler) override;
    void disconnect() override;

private:
    void processReading();
    void readData(std::size_t bytesRead);
    void sendData(const std::string& data) override;
    void onBytesWritten(std::size_t numOfBytes);
    void close();

private:
  unixdomain::socket socket_;
  ErrorHandler errorHandler_;
  std::array<char, 65536> buffer_;
  std::string writeBuffer_;
  std::size_t bytesToWrite_ = 0;
  bool disconnectRequested_ = false;
  bool isConnected_ = false;
};

}
