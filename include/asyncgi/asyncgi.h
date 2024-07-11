#ifndef ASYNCGI_H
#define ASYNCGI_H

#include "asiodispatcher.h"
#include "client.h"
#include "errors.h"
#include "events.h"
#include "io.h"
#include "router.h"
#include "server.h"
#include "timer.h"

#ifndef ASYNCGI_DISABLE_GLOBAL_HTTP_NAMESPACE
namespace http = asyncgi::http;
#endif

#ifndef ASYNCGI_DISABLE_GLOBAL_FASTCGI_NAMESPACE
namespace fastcgi = asyncgi::fastcgi;
#endif

#endif //ASYNCGI_H