#ifndef ASYNCGI_EVENTS_H
#define ASYNCGI_EVENTS_H

namespace asyncgi {

enum ErrorEvent {
    ConnectionError,
    SocketReadError,
    SocketWriteError,
    SocketCloseError,
    RequestProcessingError,
    RouteParametersError
};

} //namespace asyncgi

#endif //ASYNCGI_EVENTS_H
