#pragma once
#include "alias_unixdomain.h"
#include "timerprovider.h"
#include <asyncgi/iclient.h>
#include <asyncgi/itimer.h>
#include <asyncgi/errors.h>
#include <hot_teacup/request.h>
#include <hot_teacup/response.h>
#include <fcgi_responder/requester.h>
#include <filesystem>

namespace asyncgi::detail{
namespace fs = std::filesystem;
class TimerProvider;

class Client : public IClient, public fcgi::Requester {
public:
    Client(asio::io_context&, ErrorHandlerFunc);
    void makeRequest(
            const fs::path& socketPath,
            std::map<std::string, std::string> fcgiParams,
            std::string fcgiStdIn,
            std::function<void(const std::optional<std::string>&)> responseHandler,
            const std::chrono::milliseconds& timeout) override;
    void makeRequest(
            const fs::path& socketPath,
            const http::Request& request,
            const std::function<void(const std::optional<http::Response>&)>& responseHandler,
            const std::chrono::milliseconds& timeout) override;
    void disconnect() override;

private:
    void makeRequest(
            const fs::path& socketPath,
            std::map<std::string, std::string> fcgiParams,
            std::string fcgiStdIn,
            std::function<void(const std::optional<fcgi::ResponseData>&)> responseHandler,
            const std::shared_ptr<std::function<void()>>& cancelRequestOnTimeout);
    void processReading();
    void readData(std::size_t bytesRead);
    void sendData(const std::string& data) override;
    void onBytesWritten(std::size_t numOfBytes);
    void close();

private:
  unixdomain::socket socket_;
  TimerProvider timerProvider_;
  ErrorHandler errorHandler_;
  std::array<char, 65536> buffer_;
  std::string writeBuffer_;
  std::size_t bytesToWrite_ = 0;
  bool disconnectRequested_ = false;
};

}
