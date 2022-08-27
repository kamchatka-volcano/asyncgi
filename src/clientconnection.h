#pragma once
#include <asyncgi/types.h>
#include <asyncgi/itimer.h>
#include <asyncgi/errors.h>
#include <hot_teacup/request.h>
#include <hot_teacup/response.h>
#include <fcgi_responder/requester.h>
#include <fcgi_responder/fcgi_limits.h>
#include <asio/basic_stream_socket.hpp>
#include <filesystem>

namespace asyncgi::detail{
namespace fs = std::filesystem;
class TimerProvider;

template<typename TProtocol>
class ClientConnection : public fcgi::Requester {
public:
    ClientConnection(asio::io_context&, ErrorHandlerFunc);
    ~ClientConnection() override;

    void makeRequest(
            const typename TProtocol::endpoint& socketPath,
            fastcgi::Request request,
            std::function<void(std::optional<fcgi::ResponseData>)> responseHandler,
            const std::shared_ptr<std::function<void()>>& cancelRequestOnTimeout);
    void disconnect() override;

private:
    void processReading();
    void readData(std::size_t bytesRead);
    void sendData(const std::string& data) override;
    void onBytesWritten(std::size_t numOfBytes);
    void close();

private:
  asio::basic_stream_socket<TProtocol> socket_;
  ErrorHandler errorHandler_;
  std::array<char, fcgi::maxRecordSize> buffer_;
  std::string writeBuffer_;
  std::string nextWriteBuffer_;
  std::size_t bytesToWrite_ = 0;
  bool disconnectRequested_ = false;
};

}
