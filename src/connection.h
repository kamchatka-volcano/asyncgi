#pragma once
#include "asiodispatcherservice.h"
#include "clientservice.h"
#include "responsesender.h"
#include "timerprovider.h"
#include <asyncgi/detail/asio_namespace.h>
#ifdef ASYNCGI_USE_BOOST_ASIO
#include <boost/asio/basic_stream_socket.hpp>
#else
#include <asio/basic_stream_socket.hpp>
#endif
#include <asyncgi/detail/eventhandlerproxy.h>
#include <asyncgi/detail/external/sfun/interface.h>
#include <asyncgi/errors.h>
#include <asyncgi/requestprocessor.h>
#include <fcgi_responder/fcgi_limits.h>
#include <fcgi_responder/request.h>
#include <fcgi_responder/responder.h>
#include <fcgi_responder/response.h>
#include <array>
#include <memory>
#include <optional>

namespace ASYNCGI_ASIO {
class io_context;
}

namespace asyncgi::detail {
class ConnectionFactory;
class ResponseContext;

template<typename TProtocol>
class Connection : public std::enable_shared_from_this<Connection<TProtocol>>,
                   public fcgi::Responder {
public:
    Connection(RequestProcessor, asio::io_context&, EventHandlerProxy&, sfun::access_permission<ConnectionFactory>);
    ~Connection() override;
    Connection(const Connection<TProtocol>&) = delete;
    Connection& operator=(const Connection<TProtocol>&) = delete;
    Connection(Connection<TProtocol>&&) noexcept = default;
    Connection& operator=(Connection<TProtocol>&&) noexcept = default;

    asio::basic_socket<TProtocol>& socket();
    void process();
    void readData(std::size_t bytesRead);
    void sendData(const std::string& data) final;
    void disconnect() final;
    void processRequest(fcgi::Request&& request, fcgi::Response&& response) final;

private:
    void onBytesWritten(std::size_t numOfBytes);
    void close();

private:
    std::shared_ptr<ResponseContext> responseContext_;
    std::optional<fcgi::Request> fcgiRequest_;
    std::optional<ResponseSender> responseSender_;
    RequestProcessor requestProcessor_;
    AsioDispatcherService asioDispatcher_;
    TimerProvider timerProvider_;
    ClientService client_;
    asio::basic_stream_socket<TProtocol> socket_;
    std::array<char, fcgi::maxRecordSize> buffer_;
    std::string writeBuffer_;
    std::string nextWriteBuffer_;
    std::size_t bytesToWrite_ = 0;
    bool disconnectRequested_ = false;
    sfun::member<EventHandlerProxy&> eventHandler_;
};

} // namespace asyncgi::detail
