#pragma once
#include <asyncgi/detail/asio_namespace.h>
#ifdef ASYNCGI_USE_BOOST_ASIO
#include <boost/asio/basic_stream_socket.hpp>
#else
#include <asio/basic_stream_socket.hpp>
#endif
#include <asyncgi/detail/eventhandlerproxy.h>
#include <asyncgi/detail/external/sfun/member.h>
#include <asyncgi/errors.h>
#include <asyncgi/types.h>
#include <fcgi_responder/fcgi_limits.h>
#include <fcgi_responder/requester.h>
#include <hot_teacup/request.h>
#include <hot_teacup/response.h>
#include <filesystem>

namespace asyncgi::detail {
namespace fs = std::filesystem;
class TimerProvider;

template<typename TProtocol>
class ClientConnection : public fcgi::Requester {
public:
    ClientConnection(asio::io_context&, EventHandlerProxy&);
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
    sfun::member<EventHandlerProxy&> eventHandler_;
    std::array<char, fcgi::maxRecordSize> buffer_;
    std::string writeBuffer_;
    std::string nextWriteBuffer_;
    std::size_t bytesToWrite_ = 0;
    bool disconnectRequested_ = false;
};

} // namespace asyncgi::detail
