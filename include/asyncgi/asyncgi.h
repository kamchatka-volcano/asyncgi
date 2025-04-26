#ifndef ASYNCGI_H
#define ASYNCGI_H

#include "asio_context.h"
#include "asiodispatcher.h"
#include "errors.h"
#include "events.h"
#include "fastcgi_client.h"
#include "io.h"
#include "router.h"
#include "server.h"
#include "timer.h"
#ifdef ASYNCGI_USE_HTTP_CLIENT
#include "http_client.h"
#endif

#ifndef ASYNCGI_DISABLE_GLOBAL_HTTP_NAMESPACE
namespace http = asyncgi::http;
#endif

#ifndef ASYNCGI_DISABLE_GLOBAL_FASTCGI_NAMESPACE
namespace fastcgi = asyncgi::fastcgi;
#endif

#endif //ASYNCGI_H